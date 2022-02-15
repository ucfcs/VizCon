#include "vcuserlibrary.h"


// Lists used to track all threads and semaphores
CSThread *vizconThreadList, *vizconThreadListHead;

// Create a thread instance with arguments
void vcThreadQueue(threadFunc func, void *arg)
{
    // Attempts to create the thread
    CSThread *thread = createThread(func, arg);

    // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadList == NULL)
    {
        thread->name = vizconCreateName(0, 0);
        thread->num = 0;
        vizconThreadListHead = thread;
        vizconThreadList = thread;
    }
    else
    {
        thread->name = vizconCreateName(0, vizconThreadList->num + 1);
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

/* Create a thread instance with arguments, takes in a third
parameter allowing the user to assign a name to the thread */
void vcThreadQueueNamed(threadFunc func, void *arg, char *name)
{
    CSThread *thread = createThread(func, arg);

    // Attempts to allocate space for the thread name
    char *mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL)
    {
        vizconError("vcThreadQueueNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    thread->name = mallocName;

        // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListHead == NULL)
    {
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListHead = thread;
    }
    else
    {
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

// Start all threads created by vcThreadQueue
void vcThreadStart()
{
    // If there are no threads in the list, return immediately
    if (vizconThreadListHead == NULL)
    {
        return;
    }

    // Begin all threads
    vizconThreadList = vizconThreadListHead;
    

    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    // Waits for all threads to complete
    vizconThreadList = vizconThreadListHead;
    
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }
    

    freeUserThreads();
}

// Start all threads created by vcThreadQueue and return their results
THREAD_RET *vcThreadReturn()
{
    int i = 0;

    if (vizconThreadListHead == NULL)
        return NULL;

    // Begin all threads and get the number of threads
    vizconThreadList = vizconThreadListHead;
    
    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    THREAD_RET *arr = (THREAD_RET*)malloc(sizeof(THREAD_RET) * i);

    // Wait for all threads to complete and get return values
    vizconThreadList = vizconThreadListHead;

    i = 0;
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        arr[i] = vizconThreadList->returnVal;
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    freeUserThreads();
    return arr;
}

// Frees all user threads
void freeUserThreads()
{
    while(vizconThreadListHead != NULL)
    {
        vizconThreadList = vizconThreadListHead->next;
        free(vizconThreadListHead->name);
        freeThread(vizconThreadListHead);
        vizconThreadListHead = vizconThreadList;
    }
}

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
}

// vcMutexTrylock - Try to obtain the mutex.
//                  If it's unavailable, return without waiting.
//                  Returns: 1 if the mutex was obtained, 0 otherwise.
int vcMutexTrylock(VCMutex* mutex)
{
    return mutexTryLock(mutex->mutex);
}

// vcMutexUnlock - Release a mutex lock.
void vcMutexUnlock(VCMutex* mutex)
{
    mutexUnlock(mutex->mutex);
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