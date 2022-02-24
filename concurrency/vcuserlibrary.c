#include "vcuserlibrary.h"

// Pointers used to track all concurrency objects.
CSThread *vizconThreadListHead, *vizconThreadList;
CSSem    *vizconSemListHead,    *vizconSemList;
CSMutex  *vizconMutexListHead,  *vizconMutexList;

// vcThreadQueue - Prepare a thread instance with the function and arguments.
//                 Automatically generate the thread name.
void vcThreadQueue(threadFunc func, void *arg)
{
    // Attempt to create the thread.
    CSThread *thread = createThread(func, arg);

    // If there are no threads in the list, make the new thread the initial one.
    if (vizconThreadList == NULL)
    {
        thread->name = vizconCreateName(VC_TYPE_THREAD, 0);
        thread->num = 0;
        vizconThreadListHead = thread;
        vizconThreadList = thread;
    }

    // Otherwise, add the thread to the end of the list.
    else
    {
        thread->name = vizconCreateName(VC_TYPE_THREAD, vizconThreadList->num + 1);
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

// vcThreadQueueNamed - Prepare a thread instance with the name, function, and arguments.
void vcThreadQueueNamed(threadFunc func, void *arg, char *name)
{
    CSThread *thread = createThread(func, arg);

    // Attempt to allocate space for the thread name.
    char *mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL)
        vizconError("vcThreadQueueNamed", VC_ERROR_MEMORY);
    
    sprintf(mallocName, "%s", name);
    thread->name = mallocName;

    // If there are no threads in the list, make the new thread the initial one.
    if (vizconThreadListHead == NULL)
    {
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListHead = thread;
    }

    // Otherwise, add the thread to the end of the list.
    else
    {
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

// vcThreadStart - Start all threads created by vcThreadQueue and vcThreadQueueNamed.
void vcThreadStart()
{
    // If there are no threads in the list, return immediately.
    if (vizconThreadListHead == NULL)
        return;

    // Begin all threads.
    vizconThreadList = vizconThreadListHead;
    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    // Wait for all threads to complete.
    vizconThreadList = vizconThreadListHead;
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }
    
    // Free all the threads.
    freeUserThreads();
    // FIXME: Free threads and semaphores?
}

// vcThreadReturn - Start all threads created by vcThreadQueue and vcThreadQueueNamed.
//                  Returns: an array of all values returned by the threads.
THREAD_RET *vcThreadReturn()
{
    // If there are no threads in the list, return immediately.
    if (vizconThreadListHead == NULL)
        return NULL;

    // Begin all threads and determine the number of threads.
    int i = 0;
    vizconThreadList = vizconThreadListHead;
    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    // Allocate the array of return values.
    THREAD_RET* arr = (THREAD_RET*) malloc(sizeof(THREAD_RET) * i);

    // Wait for all threads to complete and save the return values.
    vizconThreadList = vizconThreadListHead;
    i = 0;
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        arr[i] = vizconThreadList->returnVal;
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    // Free all the threads.
    freeUserThreads();
    // FIXME: Free threads and semaphores?

    return arr;
}

// freeUserThreads - Free every thread in the thread list.
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

// vcSemCreate - Create a semaphore with the specified maximum permit count.
//               Returns: a pointer to the new semaphore.
vcSem* vcSemCreate(int count)
{
    // Make sure the count is valid.
    if (count <= 0)
        vizconError("vcSemCreate", VC_ERROR_BADCOUNT);

    // If there are no semaphores in the list, make the new semaphore the initial one.
    vcSem* sem;
    if(vizconSemList == NULL)
    {
        sem = semCreate(vizconCreateName(VC_TYPE_SEM, 0), count);
        sem->num = 0;
        vizconSemListHead = sem;
        vizconSemList = sem;
    }

    // Otherwise, add the semaphore to the end of the list.
    else
    {
        sem = semCreate(vizconCreateName(VC_TYPE_SEM, vizconSemList->num + 1), count);
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

// vcSemCreateNamed - Create a semaphore with the specified name and maximum permit count.
//                    Returns: a pointer to the new semaphore.
vcSem* vcSemCreateNamed(int count, char* name)
{
    // Make sure the count is valid.
    if (count <= 0)
        vizconError("vcSemCreate", VC_ERROR_BADCOUNT);

    // Attempt to allocate space for the semaphore name.
    char* mallocName = (char*) malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL) 
        vizconError("vcSemCreateNamed", VC_ERROR_MEMORY);
    
    sprintf(mallocName, "%s", name);
    vcSem* sem = semCreate(mallocName, count);

    // If there are no semaphores in the list, make the new semaphore the initial one.
    if(vizconSemList == NULL)
    {
        sem->num = 0;
        vizconSemListHead = sem;
        vizconSemList = sem;
    }

    // Otherwise, add the semaphore to the end of the list.
    else
    {
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

// vcSemWait - Obtain a permit from the semaphore.
//             If a permit is not available yet, wait until it is.
void vcSemWait(vcSem* sem)
{
    semWait(sem);
}

// vcSemWaitMult - Obtain the specified number of permits from the semaphore.
//                 If the permits are not available yet, wait until they are.
void vcSemWaitMult(vcSem* sem, int num)
{
    int i;
    for(i = 0; i < num; i++)
        semWait(sem);
}

// vcSemTryWait - Try to obtain a permit from the semaphore.
//                If a permit is unavailable, return without waiting.
//                Returns: 1 if the permit was obtained, 0 otherwise.
int vcSemTryWait(vcSem* sem)
{
    return semTryWait(sem);
}

// vcSemTryWaitMult - Try to obtain the specified number of permits.
//                    If any permit is unavailable, return without waiting.
//                    Returns: 1 if the permits were obtained, 0 otherwise.
int vcSemTryWaitMult(vcSem* sem, int num)
{
    // If the semaphore's current value is too low, don't bother checking.
    if(vcSemValue(sem) < num)
        return 0;
    
    // Try to obtain each permit.
    // If any is not obtained, undo all previous obtains.
    int i;
    for(i = 0; i < num; i++)
    {
        if(!vcSemTryWait(sem))
        {
            for(; i > 0; i--)
                vcSemSignal(sem);
            return 0;
        }
    }
    return 1;
}

// vcSemSignal - Release one permit from the semaphore.
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
}

// vcSemSignalMult - Release the given number of permits from the semaphore.
void vcSemSignalMult(vcSem* sem, int num)
{
    int i;
    for(i = 0; i < num; i++)
        semSignal(sem);
}

// vcSemValue - Check the amount of permits the given semaphore has available.
//              Returns: the number of available permits.
int vcSemValue(vcSem* sem)
{
    return semValue(sem);
}

// closeAllSemaphores - Close and destroy all semaphores.
//                      For use in vcThreadStart and vcThreadReturn.
void closeAllSemaphores()
{
    // Keep removing the list head until the whole list is empty.
    while(vizconSemListHead != NULL)
    {
        vizconSemList = vizconSemListHead->next;
        free(vizconSemListHead->name);
        semClose(vizconSemListHead);
        vizconSemListHead = vizconSemList;
    }
}

// vcMutexCreate - Create a mutex and add it to the list.
//                 Returns: a pointer to the mutex list entry.
CSMutex* vcMutexCreate()
{
    // Define the mutex.
    CSMutex* mutex;

    // The mutex name is dependent on whether the list is empty.
    // Create the mutex once the appropriate name can be determined.
    // Then, if the list is empty, set the new mutex as the head.
    if(vizconMutexList == NULL)
    {
        mutex = mutexCreate(vizconCreateName(VC_TYPE_MUTEX, 0));
        mutex->num = 0;
        vizconMutexListHead = mutex;
        vizconMutexList = mutex;
    }

    // Otherwise, add it to the end of the list.
    else
    {
        mutex = mutexCreate(vizconCreateName(VC_TYPE_MUTEX, vizconMutexList->num + 1));
        mutex->num = vizconMutexList->num + 1;
        vizconMutexList->next = mutex;
        vizconMutexList = mutex;
    }
    return mutex;
}

// vcMutexCreateNamed - Create a mutex with the given name and add it to the list.
//                      Returns: a pointer to the mutex list entry.
CSMutex* vcMutexCreateNamed(char* name)
{
    // Attempt to allocate space for the mutex name.
    char* mallocName = (char*) malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL) 
        vizconError("vcMutexCreateNamed", VC_ERROR_MEMORY);
    sprintf(mallocName, "%s", name);

    // Create the mutex.
    CSMutex* mutex = mutexCreate(mallocName);

    // If the list is empty, set this as the head.
    if(vizconMutexList == NULL)
    {
        mutex->num = 0;
        vizconMutexListHead = mutex;
        vizconMutexList = mutex;
    }

    // Otherwise, add it to the end of the list.
    else
    {
        mutex->num = vizconMutexList->num + 1;
        vizconMutexList->next = mutex;
        vizconMutexList = mutex;
    }
    return mutex;
}

// vcMutexLock - Obtain a lock on the mutex.
//               If a lock is not available yet, wait until it is.
void vcMutexLock(CSMutex* mutex)
{
    mutexLock(mutex);
}

// vcMutexTrylock - Try to obtain the mutex.
//                  If it's unavailable, return without waiting.
//                  Returns: 1 if the mutex was obtained, 0 otherwise.
int vcMutexTrylock(CSMutex* mutex)
{
    return mutexTryLock(mutex);
}

// vcMutexUnlock - Release a mutex lock.
void vcMutexUnlock(CSMutex* mutex)
{
    mutexUnlock(mutex);
}

// vcMutexStatus - Check whether the mutex is available.
//                 Returns: 1 if lock is available, 0 otherwise.
int vcMutexStatus(CSMutex* mutex)
{
    return mutexStatus(mutex);
}

// closeAllMutexes - Close and destroy all mutexes.
//                   For use in vcThreadStart and vcThreadReturn.
void closeAllMutexes()
{
    // Keep removing the list head until the whole list is empty.
    while(vizconMutexListHead != NULL)
    {
        vizconMutexList = vizconMutexListHead->next;
        free(vizconMutexListHead->name);
        mutexClose(vizconMutexListHead);
        vizconMutexListHead = vizconMutexList;
    }
}