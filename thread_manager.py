from random import Random, randint
import sys

def debug_print(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

seed = randint(0, 10000)
debug_print("Using RNG seed", seed)
thread_scheduler_rng = Random(seed)

class ThreadManager:
    waiting_dict2 = {}
    managed_threads = []
    exited_threads = set()
    semaphoreMap = {}
    semWaitLists = {}
    nextThreadID = 2
    def __init__(self, main_lldb_thread):
        main_thread = {'thread': main_lldb_thread, 'pthread_id': '#main_thread', 'name': '#main_thread', 'state': 'ready'}
        self.ready_list2 = [main_thread]
        self.managed_threads.append(main_thread)

    def __lookupFromLLDB(self, lldb_thread):
        for t in self.managed_threads:
            if t['thread'].GetThreadID() == lldb_thread.GetThreadID():
                return t
        debug_print("Thread lookup failed!", lldb_thread)
    def onJoin(self, lldb_thread, joined_on):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        already_exited = joined_on in self.exited_threads
        debug_print("Thread join:", c_thread['pthread_id'], "joined", joined_on, "(already exited)" if already_exited else "(not exited)")
        if not already_exited:
            if not joined_on in self.waiting_dict2:
                self.waiting_dict2[joined_on] = []
            self.waiting_dict2[joined_on].append(c_thread)
            self.ready_list2.remove(c_thread)
            c_thread['state'] = 'waiting (vcJoin)'
    def getSemaphoreValue(self, sem):
        if sem not in self.semaphoreMap:
            return None
        return self.semaphoreMap[sem]
    def onWaitSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Semaphore wait:", c_thread['pthread_id'], "waited on", sem)
        old_val = self.semaphoreMap[sem]
        if old_val > 0:
            self.semaphoreMap[sem] -= 1
            debug_print("Wait: sem value updated (no wait) from", old_val, "to", self.semaphoreMap[sem])
            return
        if not sem in self.semWaitLists:
            self.semWaitLists[sem] = []
        self.semWaitLists[sem].append(c_thread)
        self.ready_list2.remove(c_thread)
        c_thread['state'] = 'waiting (sempahore)'
    def onSignalSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        old_val = self.semaphoreMap[sem]
        self.semaphoreMap[sem] += 1
        debug_print("Signal: sem value updated from", old_val, "to", self.semaphoreMap[sem])
        if not sem in self.semWaitLists:
            return
        while self.semaphoreMap[sem] > 0:
            if len(self.semWaitLists[sem]) == 0:
                return
            self.semaphoreMap[sem] -= 1
            # TODO: there's an api to just choose k. no need to loop
            woken_thread = thread_scheduler_rng.choice(self.semWaitLists[sem])
            self.semWaitLists[sem].remove(woken_thread)
            debug_print("\tAdding back", woken_thread['pthread_id'], "to the ready list")
            self.ready_list2.append(woken_thread)
            woken_thread['state'] = 'ready'

    def onCreateThread(self, thread):
        thread['state'] = 'ready'
        thread['name'] = str(self.nextThreadID)
        self.nextThreadID += 1
        self.managed_threads.append(thread)
        self.ready_list2.append(thread)
        debug_print("Created thread:", thread['pthread_id'])

    def chooseThread(self):
        if len(self.ready_list2) <= 0:
            return None
        chosen_thread = thread_scheduler_rng.choice(self.ready_list2)
        return chosen_thread

    def onExitThread(self, lldb_thread):
        t = self.__lookupFromLLDB(lldb_thread)
        self.exited_threads.add(t['pthread_id'])
        self.ready_list2.remove(t)
        debug_print("Thread exit:", t['pthread_id'])
        if t['pthread_id'] in self.waiting_dict2:
            waiting_threads = self.waiting_dict2[t['pthread_id']]
            del self.waiting_dict2[t['pthread_id']]
            for w in waiting_threads:
                debug_print("\tAdding back", w['pthread_id'], "to the ready list")
                self.ready_list2.append(w)
            # TODO: it might be appropriate to resume that thread here and now
        else:
            debug_print("\tNo thread is waiting on it")
        self.managed_threads.remove(t)
    def registerSem(self, sem):
        # sem is a string for now
        self.semaphoreMap[sem] = 1
    
    def getManagedThreads(self):
        return self.managed_threads