#include "semaphores.h"

//Create a CSSem
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue)
{
    if(name == NULL)
    {
        return NULL;
    }
    CSSem* sem = (CSSem*)malloc(sizeof(CSSem));
    if (sem == NULL) 
        vizconError("vcSemCreate/vcSemCreateNamed", VC_ERROR_MEMORY);
    
    sem->next = NULL;
    sem->name = (char*)name;
    sem->num = -1;
    #if defined(_WIN32) // windows
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    if(sem->sem == NULL)
    {
        int err = (int)GetLastError();
        free(sem);
        vizconError("vcSemCreate/vcSemCreateNamed", err);
    }
    sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    if(sem->sem == SEM_FAILED)
    {
        free(sem);
        vizconError("vcSemCreate/vcSemCreateNamed", errno);
    }
    if(sem_unlink(name))
    {
        free(sem);
        vizconError("vcSemCreate/vcSemCreateNamed", errno);
    }
    sem->count = maxValue;
    #endif
    return sem;
}

//Releases 1 permit from semaphore and increment its count
void semSignal(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!ReleaseSemaphore(sem->sem, 1, NULL))
        vizconError("vcSemSignal/vcSemSignalMult", GetLastError());
    sem->count = sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_post(sem->sem))
        vizconError("vcSemSignal/vcSemSignalMult", errno);
    sem->count = sem->count + 1;
    #endif
}

//Waits for semaphore to become available, attaining 1 permit from semaphore and decrementing its count
void semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, INFINITE);
    switch(ret)
    {
        // WAIT_FAILED: OS-level error.
        case WAIT_FAILED:
            vizconError("vcSemWait/vcSemWaitMult", GetLastError());
        
        // WAIT_OBJECT_0: Success. Decrement the count.
        case WAIT_OBJECT_0:
            sem->count = sem->count - 1;

        // WAIT_ABANDONED: A thread with a permit closed before returning it.
        //                 This is only supposed to happen to mutexes,
        //                 but it's here for safety.
        case WAIT_ABANDONED:
            vizconError("vcSemWait/vcSemWaitMult", VC_ERROR_ABANDONED);
        
        // WAIT_TIMEOUT - Thread was not available before timeout.
        //                This shouldn't happen, but it's here for safety.
        case WAIT_TIMEOUT:
            vizconError("vcSemWait/vcSemWaitMult", VC_ERROR_TIMEOUT);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_wait(sem->sem))
        vizconError("vcSemWait/vcSemWaitMult", errno);
    
    #endif
}

//Try to attain 1 permit from semaphore and decrement its count
//Returns immediately if semaphore has no remaining permits at time of call
//returns 1 if available, else 0
int semTryWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, 0);
    switch(ret)
    {
        // WAIT_OBJECT_0 - No error. Decrement the counter.
        case WAIT_OBJECT_0:
        {
            sem->count = sem->count - 1;
            return 1;
        }

        // WAIT_TIMEOUT - Semaphore was not available before timeout.
        //                Since timeout is 0, this means it has no permits.
        case WAIT_TIMEOUT:
            return 0;
        
        // WAIT_ABANDONED: A thread with a permit closed before returning it.
        //                 This is only supposed to happen to mutexes,
        //                 but it's here for safety.
        case WAIT_ABANDONED:
        {
            vizconError("vcSemTrylock/vcSemTrylockMult", VC_ERROR_ABANDONED);
            return 0;
        }
        
        // WAIT_FAILED - OS-level error.
        case WAIT_FAILED:
        {
            vizconError("vcSemTrylock/vcSemTrylockMult", GetLastError());
            return 0;
        }
    }
    
    #elif __linux__ || __APPLE__ // POSIX version
    int ret = sem_trywait(sem->sem);
    switch(ret)
    {
        // 0 - Success. Decrement the semaphore count.
        case 0:
        {
            sem->count = sem->count - 1;
            return 1;
        }

        // EBUSY - Semaphore currently has no permits. Just leave.
        case EBUSY:
            return 0;

        // Default - OS-level error. Just pass it on.
        default:
        {
            vizconError("vcSemTrylock/vcSemTrylockMult", ret);
            return 0;
        }
    }
    #endif
    return 0;
}

//Returns semaphore's current value
int semValue(CSSem* sem)
{
    return sem->count;
}

//Frees all data associated with a CSSem type, including itself
void semClose(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!CloseHandle(sem->sem))
    {
        // FIXME: Change referenced function.
        vizconError("vcSemClose", GetLastError());
    }
    free(sem);
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_close(sem->sem))
    {
        // FIXME: Change referenced function.
        vizconError("vcSemClose", errno);
    }
    free(sem);
    #endif
}