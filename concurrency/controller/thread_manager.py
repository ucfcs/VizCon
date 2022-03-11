from random import Random, randint
import sys

def debug_print(*args, **kwargs):
    return
    #print(*args, file=sys.stderr, **kwargs)

seed = randint(0, 10000)
debug_print("Using RNG seed", seed)
thread_scheduler_rng = Random(seed)

class ThreadManager:
    waiting_dict2 = {}
    managed_threads = []
    exited_threads = set()
    semaphoreMap = {}
    semaphoreMapByName = {}
    semWaitLists = {}
    nextThreadID = 2
    def __init__(self, main_lldb_thread):
        main_thread = {'thread': main_lldb_thread, 'name': '#main_thread', 'state': 'ready', 'csthread_ptr': 'N/A'}
        self.ready_list2 = [main_thread]
        self.managed_threads.append(main_thread)

    def __lookupFromLLDB(self, lldb_thread):
        for t in self.managed_threads:
            if t['thread'].GetThreadID() == lldb_thread.GetThreadID():
                return t
        debug_print("Thread lookup failed!", lldb_thread)
    def __getThreadID(self, c_thread):
        return str(c_thread['thread'].GetIndexID())
    def __lookupFromCSThreadPtr(self, csthread_ptr):
        for thread in self.managed_threads:
            if thread['csthread_ptr'] == csthread_ptr:
                return thread
        debug_print("Thread lookup by pointer failed!", csthread_ptr, self.managed_threads)
        return None
    def onJoin(self, lldb_thread, joined_on_ptr):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        c_thread_joined_on = self.__lookupFromCSThreadPtr(joined_on_ptr)
        joined_on_id = self.__getThreadID(c_thread_joined_on)
        already_exited = joined_on_id in self.exited_threads
        debug_print("Thread join:", c_thread['name'], "joined", joined_on_id, "(already exited)" if already_exited else "(not exited)")
        if not already_exited:
            if not joined_on_id in self.waiting_dict2:
                self.waiting_dict2[joined_on_id] = []
            self.waiting_dict2[joined_on_id].append(c_thread)
            self.ready_list2.remove(c_thread)
            c_thread['state'] = 'waiting (vcJoin)'
    def getSemaphoreValue(self, sem):
        if sem not in self.semaphoreMap:
            return None
        return self.semaphoreMap[sem]['value']
    def onWaitSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Semaphore wait:", c_thread['name'], "waited on", sem)
        sem_obj = self.semaphoreMap[sem]
        old_val = sem_obj['value']
        if old_val > 0:
            sem_obj['value'] -= 1
            debug_print("Wait: sem value updated (no wait) from", old_val, "to", sem_obj['value'])
            return
        if not sem in self.semWaitLists:
            self.semWaitLists[sem] = []
        self.semWaitLists[sem].append(c_thread)
        self.ready_list2.remove(c_thread)
        c_thread['state'] = 'waiting (sempahore)'
    def onSignalSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        sem_obj = self.semaphoreMap[sem]
        old_val = sem_obj['value']
        if old_val >= sem_obj['max_value']:
            debug_print("Unimplemented error handling: semaphore max value reached")
            sys.exit(1)
        sem_obj['value'] += 1
        debug_print("Signal: sem value updated from", old_val, "to", sem_obj['value'])
        if not sem in self.semWaitLists:
            return
        while sem_obj['value'] > 0:
            if len(self.semWaitLists[sem]) == 0:
                return
            sem_obj['value'] -= 1
            # TODO: there's an api to just choose k. no need to loop
            woken_thread = thread_scheduler_rng.choice(self.semWaitLists[sem])
            self.semWaitLists[sem].remove(woken_thread)
            debug_print("\tAdding back", woken_thread['name'], "to the ready list")
            self.ready_list2.append(woken_thread)
            woken_thread['state'] = 'ready'

    def onCreateThread(self, thread):
        thread['state'] = 'ready'
        self.nextThreadID += 1
        self.managed_threads.append(thread)
        self.ready_list2.append(thread)
        debug_print("Created thread:", thread['name'])

    def chooseThread(self):
        if len(self.ready_list2) <= 0:
            return None
        chosen_thread = thread_scheduler_rng.choice(self.ready_list2)
        return chosen_thread

    def onExitThread(self, lldb_thread):
        t = self.__lookupFromLLDB(lldb_thread)
        thread_id = self.__getThreadID(t)
        self.exited_threads.add(thread_id)
        self.ready_list2.remove(t)
        debug_print("Thread exit:", t['name'])
        t['state'] = 'completed'
        if thread_id in self.waiting_dict2:
            waiting_threads = self.waiting_dict2[thread_id]
            del self.waiting_dict2[thread_id]
            for w in waiting_threads:
                debug_print("\tAdding back", w['name'], "to the ready list")
                self.ready_list2.append(w)
            # TODO: it might be appropriate to resume that thread here and now
        else:
            debug_print("\tNo thread is waiting on it")
        #self.managed_threads.remove(t)
    def registerSem(self, sem, new_sem_name, new_sem_initial_value, new_sem_max_value):
        # sem is a string for now
        if new_sem_name in self.semaphoreMapByName:
            debug_print("Unimplemented error handling: duplicate semaphore name")
            sys.exit(1)
        self.semaphoreMap[sem] = {'value': new_sem_initial_value, 'name': new_sem_name, 'max_value': new_sem_max_value}
        self.semaphoreMapByName[new_sem_name] = self.semaphoreMap[sem]
    
    def getManagedThreads(self):
        return self.managed_threads