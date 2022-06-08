from random import Random, randint
from dataclasses import dataclass, field
from typing import Any
import time
import heapq

from concurrency.controller.utils import CrossThreadUnlockError, DoubleLockError, SemValueLimitError

@dataclass(order=True)
class SleepEntry:
    wakeup_time: int
    c_thread: Any=field(compare=False)

def debug_print(*args, **kwargs):
    return
    #print(*args, file=sys.stderr, **kwargs)
    #sys.stderr.flush()
    #sys.stdout.flush()

seed = randint(0, 10000)
debug_print("Using RNG seed", seed)
thread_scheduler_rng = Random(seed)

class ThreadManager:
    waiting_dict2 = {}
    managed_threads = []
    exited_threads = set()
    semaphoreMap = {}
    semWaitLists = {}
    mutexMap = {}
    sleeping_threads = []
    def __init__(self, main_lldb_thread):
        main_thread = {'thread': main_lldb_thread, 'name': 'Main Thread', 'state': 'ready', 'csthread_ptr': 'N/A'}
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
    #def __lookupFromTID(self, tid):
    #    for t in self.managed_threads:
    #        if self.__getThreadID(t) == tid:
    #            return t
    #    return None
    #def getSemaphoreDebug(self, sem):
    #    if sem not in self.semaphoreMap:
    #        return None
    #    sem_obj = self.semaphoreMap[sem]
    #    data = "Value: " + str(sem_obj['value']) + "\nPtr: "+sem
    #    for tid, val in sem_obj['threads'].items():
    #        c_thread = self.__lookupFromTID(tid)
    #        if c_thread is None:
    #            data += "\nErr thread"
    #        else:
    #            data += "\n\"" + c_thread['name']+"\": " + str(val)
    #    return data
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
            #tid = self.__getThreadID(c_thread)
            #if tid not in sem_obj['threads']:
            #    sem_obj['threads'][tid] = 0
            #sem_obj['threads'][tid] -= 1
            debug_print("\tWait: sem value updated (no wait) from", old_val, "to", sem_obj['value'])
            return
        if not sem in self.semWaitLists:
            self.semWaitLists[sem] = []
        self.semWaitLists[sem].append(c_thread)
        self.ready_list2.remove(c_thread)
        c_thread['state'] = 'waiting (sempahore)'
    def onTryWaitSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Semaphore wait:", c_thread['name'], "try waited on", sem)
        sem_obj = self.semaphoreMap[sem]
        old_val = sem_obj['value']
        if old_val > 0:
            sem_obj['value'] -= 1
            return True
        return False
    def onSignalSem(self, lldb_thread, sem):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        sem_obj = self.semaphoreMap[sem]
        old_val = sem_obj['value']
        if old_val >= sem_obj['max_value']:
            #debug_print("Unimplemented error handling: semaphore max value reached")
            raise SemValueLimitError
            #sys.exit(1)
        sem_obj['value'] += 1
        #tid = self.__getThreadID(c_thread)
        #if tid not in sem_obj['threads']:
        #    sem_obj['threads'][tid] = 0
        #sem_obj['threads'][tid] += 1
        debug_print("Signal:", c_thread['name'],' signalled ', sem, "from", old_val, "to", sem_obj['value'])
        if not sem in self.semWaitLists:
            return
        while sem_obj['value'] > 0:
            if len(self.semWaitLists[sem]) == 0:
                return
            sem_obj['value'] -= 1
            # TODO: there's an api to just choose k. no need to loop
            woken_thread = thread_scheduler_rng.choice(self.semWaitLists[sem])
            #tid = self.__getThreadID(woken_thread)
            #if tid not in sem_obj['threads']:
            #    sem_obj['threads'][tid] = 0
            #sem_obj['threads'][tid] -= 1
            self.semWaitLists[sem].remove(woken_thread)
            debug_print("\tAdding back", woken_thread['name'], "to the ready list")
            self.ready_list2.append(woken_thread)
            woken_thread['state'] = 'ready'
    def onCloseSem(self, lldb_thread, sem):
        del self.semaphoreMap[sem]
        if sem in self.semWaitLists:
            del self.semWaitLists[sem]
        debug_print("Semaphore closed")
    def onCreateThread(self, thread):
        thread['state'] = 'ready'
        self.managed_threads.append(thread)
        self.ready_list2.append(thread)
        debug_print("Created thread:", thread['name'])
    def onFreeThread(self, calling_thread, thread_ptr):
        freed_thread = self.__lookupFromCSThreadPtr(thread_ptr)
        freed_thread['csthread_ptr'] = 'N/A (freed)'
        debug_print("Thread freed")

    def onSleepThread(self, lldb_thread, millis):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Thread sleep:", c_thread['name'], "is sleeping for", millis)
        c_thread['state'] = 'sleeping'
        self.ready_list2.remove(c_thread)
        wakeup_time = round(time.time() * 1000) + millis
        sleep_entry = SleepEntry(wakeup_time, c_thread)
        heapq.heappush(self.sleeping_threads, sleep_entry)

    def chooseThread(self):
        now = round(time.time() * 1000)
        while len(self.sleeping_threads) > 0 and self.sleeping_threads[0].wakeup_time < now:
            sleep_entry = heapq.heappop(self.sleeping_threads)
            sleep_entry.c_thread['state'] = 'ready'
            self.ready_list2.append(sleep_entry.c_thread)
        if len(self.ready_list2) <= 0:
            if len(self.sleeping_threads) == 0:
                return None
        while len(self.ready_list2) <= 0:
            wakeup_time = self.sleeping_threads[0].wakeup_time
            debug_print("Empty ready list. Waiting for sleep to end")
            time.sleep((wakeup_time - now) / 1000)
            now = round(time.time() * 1000)
            while len(self.sleeping_threads) > 0 and self.sleeping_threads[0].wakeup_time < now:
                sleep_entry = heapq.heappop(self.sleeping_threads)
                sleep_entry.c_thread['state'] = 'ready'
                self.ready_list2.append(sleep_entry.c_thread)
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
    def registerSem(self, sem, new_sem_initial_value, new_sem_max_value):
        self.semaphoreMap[sem] = {'value': new_sem_initial_value, 'max_value': new_sem_max_value}
        #self.semaphoreMap[sem] = {'value': new_sem_initial_value, 'max_value': new_sem_max_value, 'threads': {}}



    # Mutexes
    def registerMutex(self, mutex):
        self.mutexMap[mutex] = {'locked_by': None, 'waiting': []}
    def getMutexOwner(self, mutex):
        if mutex not in self.mutexMap:
            return None
        return self.mutexMap[mutex]['locked_by']
    def onLockMutex(self, lldb_thread, mutex):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Mutex lock:", c_thread['name'], "waited on", mutex)
        mutex_obj = self.mutexMap[mutex]
        if mutex_obj['locked_by'] is None:
            mutex_obj['locked_by'] = c_thread
            debug_print("lockMutex: immediately acquired unlocked mutex")
            return
        if mutex_obj['locked_by'] == c_thread:
            debug_print("A thread attempted to lock a mutex that it already owns")
            raise DoubleLockError
            #sys.exit(1)
        mutex_obj['waiting'].append(c_thread)
        self.ready_list2.remove(c_thread)
        c_thread['state'] = 'waiting (mutex)'
    def onUnlockMutex(self, lldb_thread, mutex):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        mutex_obj = self.mutexMap[mutex]
        if mutex_obj['locked_by'] != c_thread:
            debug_print("A thread attempted to unlock a mutex that it does not own")
            #sys.exit(1)
            raise CrossThreadUnlockError
        debug_print("unlockMutex: Mutex unlocked")
        mutex_obj['locked_by'] = None
        if len(mutex_obj['waiting']) >= 1:
            woken_thread = thread_scheduler_rng.choice(mutex_obj['waiting'])
            mutex_obj['locked_by'] = woken_thread
            mutex_obj['waiting'].remove(woken_thread)
            debug_print("\tAdding back", woken_thread['name'], "to the ready list")
            self.ready_list2.append(woken_thread)
            woken_thread['state'] = 'ready'
    def onTryLockMutex(self, lldb_thread, mutex):
        c_thread = self.__lookupFromLLDB(lldb_thread)
        debug_print("Mutex tryLock:", c_thread['name'], "waited on", mutex)
        mutex_obj = self.mutexMap[mutex]
        if mutex_obj['locked_by'] is None:
            mutex_obj['locked_by'] = c_thread
            return True
        if mutex_obj['locked_by'] == c_thread:
            #debug_print("A thread attempted to tryLock a mutex that it already owns")
            return True
        return False
    def onCloseMutex(self, lldb_thread, mutex):
        debug_print("Mutex closed")
        del self.mutexMap[mutex]
    
    def getManagedThreads(self):
        return self.managed_threads