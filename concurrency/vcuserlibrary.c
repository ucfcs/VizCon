#include "vcuserlibrary.h"

// Create a thread instance with arguments
void vcThreadQueue(threadFunc func, void *arg)
{
    // Attempts to create the thread
    CSThread *thread = createThread(func, arg);

    // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListInitial == NULL)
    {
        thread->name = vizconCreateName(0, 0);
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
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
    if (mallocName = NULL)
    {
        vizconError("vcThreadQueueNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    thread->name = mallocName;

        // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListInitial == NULL)
    {
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
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
    if (vizconThreadListInitial = NULL)
    {
        return;
    }

    // Begin all threads
    vizconThreadList = vizconThreadListInitial;

    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    // Waits for all threads to complete
    vizconThreadList = vizconThreadListInitial;
    
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    freeThreads();
}

// Start all threads created by vcThreadQueue and return their results
THREAD_RET *vcThreadReturn()
{
    int i = 0;

    if (vizconThreadListInitial = NULL)
    {
        return;
    }

    // Begin all threads and get the number of threads
    vizconThreadList = vizconThreadListInitial;
    
    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    THREAD_RET *arr = (THREAD_RET*)malloc(sizeof(THREAD_RET) * i);

    // Wait for all threads to complete and get return values
    vizconThreadList = vizconThreadListInitial;

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
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        free(vizconThreadListInitial->name);
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }
}

//Create a semaphore with a maximum count
//All semaphores values must be a postive integer
vcSem* vcSemCreate(int count)
{
    if(count <= 0)
    {
        vizconError("vcSemCreate", 503);
    }
    vcSem* sem;
    if(vizconSemList == NULL)
    {
        sem = semCreate(vizconCreateName(1, 0), count);
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem = semCreate(vizconCreateName(1, vizconSemList->num + 1), count);
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

//Create a semaphore with an initial count and a max count
//All semaphores must have a name, and values must be a positive integer, except initial count which may be 0
vcSem* vcSemCreateInitial(int initialCount, int maxCount)
{
    if(initialCount < 0 || maxCount <= 0)
    {
        vizconError("vcSemCreateInitial", 503);
    }
    vcSem* sem;
    if(vizconSemList == NULL)
    {
        sem = semCreate(vizconCreateName(1, 0), maxCount);
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem = semCreate(vizconCreateName(1, vizconSemList->num + 1), maxCount);
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    int i;
    for(i=0; i<maxCount-initialCount; i++)
    {
        vcSemWait(sem);
    }
    return sem;
}

//Create a semaphore with a maximum count
//All semaphores values must be a postive integer
//Takes additional second parameter for user to assign a name to this semaphore
vcSem* vcSemCreateNamed(int count, char* name)
{
    if(count <= 0)
    {
        vizconError("vcSemCreateNamed", 503);
    }
    int i;
    for(i=0; name[i] != '\0'; i++);
    char* mallocName = (char*)malloc(sizeof(char) * (i + 1));
    if (mallocName == NULL) 
    {
        vizconError("vcSemCreateNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    vcSem* sem = semCreate(mallocName, count);
    if(vizconSemList == NULL)
    {
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

//Create a semaphore with an initial count and a max count
//All semaphores must have a name, and values must be a positive integer, except initial count which may be 0
//Takes additional third parameter for user to assign a name to this semaphore
vcSem* vcSemCreateInitialNamed(int initialCount, int maxCount, char* name)
{
    if(initialCount < 0 || maxCount <= 0)
    {
        vizconError("vcSemCreateInitialNamed", 503);
    }
    int i;
    for(i=0; name[i] != '\0'; i++);
    char* mallocName = (char*)malloc(sizeof(char) * (i + 1));
    if (mallocName == NULL) 
    {
        vizconError("vcSemCreateInitialNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    vcSem* sem = semCreate(mallocName, maxCount);
    if(vizconSemList == NULL)
    {
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    for(i=0; i<maxCount-initialCount; i++)
    {
        vcSemWait(sem);
    }
    return sem;
}

//Consume one permit from a semaphore, or wait until one is available
void vcSemWait(vcSem* sem)
{
    semWait(sem);
}

//Consume a number of permits from a semaphore equal to a user-specified number
//Will not consume any permits until all are simultaneously available
void vcSemWaitMult(vcSem* sem, int num)
{
    int i;
    while(1)
    {
        while(vcSemValue(sem) < num);
        for(i=0; i<num; i++)
        {
            if(!vcSemTryWait(sem))
            {
                for(i=i; i>0; i--)
                {
                    vcSemSignal(sem);
                }
                i = -1;
                break;
            }
        }
        if(i != -1)
        {
            return;
        }
    }
}

//Consume one permit from a sempahore, or return immediately if none are available
//Returns 1 if successful, else 0
int vcSemTryWait(vcSem* sem)
{
    if(semTryWait(sem))
    {
        return 1;
    }
    return 0;
}

//Consume a number of permits from a sempahore equal to a user-specified number, or return immediately if all are not available
//Returns 1 if successful, else 0
int vcSemTryWaitMult(vcSem* sem, int num)
{
    int i;
    if(vcSemValue(sem) < num)
    {
        return 0;
    }
    for(i=0; i<num; i++)
    {
        if(!vcSemTryWait(sem))
        {
            for(i=i; i>0; i--)
            {
                vcSemSignal(sem);
            }
            return 0;
        }
    }
    return 1;
}

//Release one permit from a semaphore
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
}

//Release a number of permits from a sempahore equal to a user-specified number
void vcSemSignalMult(vcSem* sem, int num)
{
    int i;
    for(i=0; i<num; i++)
    {
        semSignal(sem);
    }
}

//Return the current number of permits from semaphore
int vcSemValue(vcSem* sem)
{
    return sem->count;
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