#include "vcuserlibrary.h"

extern int isLldbActive;
extern void vizconError(char* func, int err);

// Pointers used to track all concurrency objects.
CSThread *vizconThreadListHead, *vizconThreadList;
CSSem *vizconSemListHead, *vizconSemList;
CSMutex *vizconMutexListHead, *vizconMutexList;
int vizconCreateFlag = 0;

extern void lldb_hook_threadSleep(int milliseconds);

// Definitions for methods that close objects.
// They are defined here because a user doesn't need to access them directly.
void closeAllThreads();
void closeAllSemaphores();
void closeAllMutexes();

// vcThreadQueue - Prepare a thread instance with the function and arguments.
//                 Automatically generate the thread name.
void vcThreadQueue(threadFunc func, void *arg)
{
    // If called while threads are already running, throw error
    if(vizconCreateFlag)
    {
        vizconError("vcThreadQueue", VC_ERROR_CREATEDISABLED);
        return;
    }

    // Attempt to create the thread.
    CSThread *thread = createThread(func, arg);
    int numSize = 1, check = 0, threadNum = 1;
    if(vizconThreadList != NULL)
    {
        check = vizconThreadList->num / 10;
        threadNum = vizconThreadList->num + 1;
    }
    while(check != 0)
    {
        numSize++;
        check = check / 10;
    }
    char* name = (char*) malloc(sizeof(char) * (numSize + 9));
    if(name == NULL)
        vizconError("create function", VC_ERROR_MEMORY);
    sprintf(name, "Thread %d", threadNum);

    // If there are no threads in the list, make the new thread the initial one.
    if (vizconThreadList == NULL)
    {
        thread->name = name;
        thread->num = 1;
        vizconThreadListHead = thread;
        vizconThreadList = thread;
    }

    // Otherwise, add the thread to the end of the list.
    else
    {
        thread->name = name;
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

// vcThreadQueueNamed - Prepare a thread instance with the name, function, and arguments.
void vcThreadQueueNamed(threadFunc func, void *arg, char *name)
{
    // If called while threads are already running, throw error
    if(vizconCreateFlag)
    {
        vizconError("vcThreadQueueNamed", VC_ERROR_CREATEDISABLED);
        return;
    }
    
    CSThread *thread = createThread(func, arg);
    int i;
    for(i = 0; name[i] != '\0'; i++);

    // Attempt to allocate space for the thread name.
    char *mallocName = (char*)malloc(sizeof(char) * (i + 1));
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
    // If called while another instance is running, throw error
    if(vizconCreateFlag)
    {
        vizconError("vcThreadStart", VC_ERROR_THREADSACTIVE);
        return;
    }
    else
    {
        vizconCreateFlag = 1;
    }

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
    vizconCreateFlag = 0;
    
    // Close all the resources.
    closeAllThreads();
    closeAllSemaphores();
    closeAllMutexes();
}

// vcThreadReturn - Start all threads created by vcThreadQueue and vcThreadQueueNamed.
//                  Returns: an array of all values returned by the threads.
void** vcThreadReturn()
{
    // If called while another instance is running, throw error
    if(vizconCreateFlag)
    {
        vizconError("vcThreadReturn", VC_ERROR_THREADSACTIVE);
        return NULL;
    }
    else
    {
        vizconCreateFlag = 1;
    }

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
    void** arr = (void**) malloc(sizeof(void*) * i);

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
    vizconCreateFlag = 0;

    // Free all the resources and return.
    closeAllThreads();
    closeAllSemaphores();
    closeAllMutexes();
    return arr;
}

//vcThreadSleep - Put the calling thread to sleep
void vcThreadSleep(int milliseconds)
{
    if (isLldbActive)
    {
        lldb_hook_threadSleep(milliseconds);
        return;
    }
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds*1000);
    #endif
}

//vcThreadId - return the calling threads id
int vcThreadId()
{
    #ifdef _WIN32
    return GetCurrentThreadId();
    #else
    return (int)gettid();
    #endif
}

// closeAllThreads - Free every thread in the thread list.
void closeAllThreads()
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
CSSem* vcSemCreate(int maxCount)
{   
    // Make sure the count is valid.
    if(maxCount <= 0)
        vizconError("vcSemCreate", VC_ERROR_BADCOUNT);
    // If there are no semaphores in the list, make the new semaphore the initial one.
    CSSem* sem = semCreate(maxCount);
    if(vizconSemList == NULL)
    {
        vizconSemListHead = sem;
        vizconSemList = sem;
    }

    // Otherwise, add the semaphore to the end of the list.
    else
    {
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

// vcSemCreateInitial - Create a semaphore with the specified initial and maximum permit count.
//                      Returns: a pointer to the new semaphore.
CSSem* vcSemCreateInitial(int maxCount, int initialCount)
{   
    // Make sure the counts are valid.
    if(initialCount < 0 || maxCount <= 0 || initialCount > maxCount)
        vizconError("vcSemCreateInitial", VC_ERROR_BADCOUNT);

    // If there are no semaphores in the list, make the new semaphore the initial one.
    CSSem* sem = semCreate(maxCount);
    if(vizconSemList == NULL)
    {
        vizconSemListHead = sem;
        vizconSemList = sem;
    }

    // Otherwise, add the semaphore to the end of the list.
    else
    {
        vizconSemList->next = sem;
        vizconSemList = sem;
    }

    // Consume permits until the current count matches the desired initial count.
    int i;
    for(i = 0; i < maxCount - initialCount; i++)
        vcSemWait(sem);
    return sem;
}

// vcSemWait - Obtain a permit from the semaphore.
//             If a permit is not available yet, wait until it is.
void vcSemWait(CSSem* sem)
{
    semWait(sem);
}

// vcSemWaitMult - Obtain the specified number of permits from the semaphore.
//                 If the permits are not available yet, wait until they are.
//                 Note that the permits are only obtained as a block;
//                 if even one is unavailable, the wait will continue.
void vcSemWaitMult(CSSem* sem, int num)
{
    int i;
    while(1)
    {
        // Wait for the semaphore to have enough permits available.
        while(vcSemValue(sem) < num);

        // Now that all permits are available, try to get all of them.
        for(i = 0; i < num; i++)
        {
            // If one of the permits suddenly becomes unavailable,
            // return all the ones already acquired and restart the wait.
            if(!vcSemTryWait(sem))
            {
                for(; i > 0; i--)
                    vcSemSignal(sem);
                i = -1;
                break;
            }
        }

        // If every permit was acquired, i will be 0.
        // Otherwise, return to the start of the loop.
        if(i != -1)
            return;
    }
}

// vcSemTryWait - Try to obtain a permit from the semaphore.
//                If a permit is unavailable, return without waiting.
//                Returns: 1 if the permit was obtained, 0 otherwise.
int vcSemTryWait(CSSem* sem)
{
    return semTryWait(sem);
}

// vcSemTryWaitMult - Try to obtain the specified number of permits.
//                    If any permit is unavailable, return without waiting.
//                    Returns: 1 if the permits were obtained, 0 otherwise.
int vcSemTryWaitMult(CSSem* sem, int num)
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
void vcSemSignal(CSSem* sem)
{
    semSignal(sem);
}

// vcSemSignalMult - Release the given number of permits from the semaphore.
void vcSemSignalMult(CSSem* sem, int num)
{
    int i;
    for(i = 0; i < num; i++)
        semSignal(sem);
}

// vcSemValue - Check the amount of permits the given semaphore has available.
//              Returns: the number of available permits.
int vcSemValue(CSSem* sem)
{
    return sem->count;
}

// closeAllSemaphores - Close and destroy all semaphores.
//                      For use in vcThreadStart and vcThreadReturn.
void closeAllSemaphores()
{
    // Keep removing the list head until the whole list is empty.
    while(vizconSemListHead != NULL)
    {
        vizconSemList = vizconSemListHead->next;
        semClose(vizconSemListHead);
        vizconSemListHead = vizconSemList;
    }
}

// vcMutexCreate - Create a mutex and add it to the list.
//                 Returns: a pointer to the mutex list entry.
CSMutex* vcMutexCreate()
{
    // Define the mutex.
    CSMutex* mutex = mutexCreate();

    // The mutex name is dependent on whether the list is empty.
    // Create the mutex once the appropriate name can be determined.
    // Then, if the list is empty, set the new mutex as the head.
    if(vizconMutexList == NULL)
    {
        vizconMutexListHead = mutex;
        vizconMutexList = mutex;
    }

    // Otherwise, add it to the end of the list.
    else
    {
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
        mutexClose(vizconMutexListHead);
        vizconMutexListHead = vizconMutexList;
    }
}

// vcHalt - Close all resources and immediately exit.
void vcHalt(int exitCode)
{
    closeAllThreads();
    closeAllSemaphores();
    closeAllMutexes();
    exit(exitCode);
}