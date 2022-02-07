#include "vcuserlibrary.h"

// Lists used to track threads and semaphores.
VCMutex* vizconMutexListHead;
VCMutex* vizconMutexListTail;

// vcMutexCreate - Creates a mutex with the given name and adds it to the list.
VCMutex* vcMutexCreate(char* name)
{
    // Allocate the node and create the mutex.
    VCMutex* mutex = (VCMutex*) malloc(sizeof(VCMutex));
    mutex->mutex = mutexCreate(name);
    mutex->next = NULL;

    // If the list is empty, set this as the head.
    if(vizconMutexListTail == NULL)
    {
        vizconMutexListHead = mutex;
        vizconMutexListTail = mutex;
    }
    // Otherwise, add it to the end of the list.
    else
    {
        vizconMutexListTail->next = mutex;
        vizconMutexListTail = mutex;
    }
    return mutex;
}

// vcMutexLock - Obtain a lock on the mutex.
//               If a lock is not available yet, wait until it is.
void vcMutexLock(VCMutex* mutex)
{
    mutexLock(mutex->mutex);
    sleepThread(20);
}

// vcMutexTrylock - Try to obtain the mutex.
//                  If it's unavailable, return without waiting.
//                  Returns: 1 if the mutex was obtained, 0 otherwise.
int vcMutexTrylock(VCMutex* mutex)
{
    if(mutexTryLock(mutex->mutex))
    {
        sleepThread(20);
        return 1;
    }
    sleepThread(20);
    return 0;
}

// vcMutexUnlock - Release a mutex lock.
void vcMutexUnlock(VCMutex* mutex)
{
    mutexUnlock(mutex->mutex);
    sleepThread(20);
}

// vcMutexStatus - Check whether the mutex is available.
//                 Returns: 1 if lock is available, 0 otherwise.
int vcMutexStatus(VCMutex* mutex)
{
    return mutexStatus(mutex->mutex);
}

// closeAllMutexes - Closes and destroys all mutex list entries.
//                   For use in vcWaitForCompletion and vcWaitForReturn.
//                   FIXME: This probably belongs in a different file.
void closeAllMutexes()
{
    // Keep removing the list head until the whole list is empty.
    while(vizconMutexListHead != NULL)
    {
        vizconMutexListTail = vizconMutexListHead->next;
        mutexClose(vizconMutexListHead->mutex);
        free(vizconMutexListHead);
        vizconMutexListHead = vizconMutexListTail;
    }    
}