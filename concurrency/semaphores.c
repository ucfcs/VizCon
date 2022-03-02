#include "semaphores.h"
#include "lldb_lib.h"

extern int isLldbActive;

// semCreate - Create a semaphore with the given name and max value.
//             Returns: a pointer to the semaphore struct.
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue)
{
    // vcSemCreate and vcSemCreateNamed should ensure the mutex is named.
    // If it somehow isn't, error out.
    if (name == NULL)
    {
        vizconError("vcSemCreate/vcSemCreateNamed", VC_ERROR_NAMEERROR);
        return NULL;
    }

    // Attempt to allocate the struct. Error out on failure.
    CSSem* sem = (CSSem*) malloc(sizeof(CSSem));
    if (sem == NULL) 
        vizconError("vcSemCreate/vcSemCreateNamed", VC_ERROR_MEMORY);
    
    // Set non-semaphore properties.
    sem->next = NULL;
    sem->name = (char*) name;
    sem->num = -1;
    sem->count = maxValue;
    if (isLldbActive)
    {
        sem->sem = NULL;
        vc_internal_registerSem(sem, sem->name, sem->count, maxValue);
        return sem;
    }
    // Platform-dependent semaphore creation.
    // Create a mutex with default settings. Error out where needed.
    #ifdef _WIN32 // Windows version
        sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
        if(sem->sem == NULL)
        {
            int err = (int) GetLastError();
            free(sem);
            vizconError("vcSemCreate/vcSemCreateNamed", err);
            return NULL;
        } 
    #elif __linux__ || __APPLE__ // POSIX version
        // Use sem_open to create a named semaphore, which macOS requires.
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
    #endif
    
    return sem;
}

// semWait - Wait for sem to become available.
//           When it is, attain 1 permit and decrement its count
void semWait(CSSem* sem)
{
    if (isLldbActive)
    {
        vcWait(sem);
        // The simulation controller should ensure this executes uninterrupted
        // but the other usages might need attention
        sem->count = sem->count - 1;
        return;
    }
    platform_semWait(sem);
}

void platform_semWait(CSSem* sem)
{
    // Platform-dependent waiting.
    #ifdef _WIN32 // Windows version
        DWORD ret = WaitForSingleObject(sem->sem, INFINITE);
        switch(ret)
        {
            // WAIT_FAILED: OS-level error.
            case WAIT_FAILED:
            {
                vizconError("vcSemWait/vcSemWaitMult", GetLastError());
                break;
            }
            
            // WAIT_OBJECT_0: Success. Decrement the count.
            case WAIT_OBJECT_0:
            {
                sem->count = sem->count - 1;
                break;
            }

            // WAIT_ABANDONED: A thread with a permit closed before returning it.
            //                 This is only supposed to happen to mutexes,
            //                 but it's here for safety.
            case WAIT_ABANDONED:
            {
                vizconError("vcSemWait/vcSemWaitMult", VC_ERROR_ABANDONED);
                break;
            }
            
            // WAIT_TIMEOUT - Thread was not available before timeout.
            //                This shouldn't happen, but it's here for safety.
            case WAIT_TIMEOUT:
            {
                vizconError("vcSemWait/vcSemWaitMult", VC_ERROR_TIMEOUT);
                break;
            }
        }
    #elif __linux__ || __APPLE__ // POSIX version
        if(sem_wait(sem->sem))
            vizconError("vcSemWait/vcSemWaitMult", errno);
        sem->count = sem->count - 1;
    #endif
}

// semTryWait - Try to obtain the semaphore.
//              If it's unavailable, return without waiting.
//              Returns: 1 if permit was available, 0 otherwise.
int semTryWait(CSSem* sem)
{
    if (isLldbActive)
    {
        fprintf(stderr, "Warning: semTryWait is unimplemented!\n"); 
        return 0;
    }
    // Platform-dependent trywaiting.
    #if defined(_WIN32) // Windows version
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
        // 0 - Success. Mark mutex as unavailable.
        if(!ret)
        {
            sem->count = sem->count - 1;
            return 1;
        }
        else switch(errno)
        {
            // EAGAIN - Semaphore currently has no permits. Just leave.
            case EAGAIN:
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

// semSignal - Release 1 permit from sem and increment its count.
void semSignal(CSSem* sem)
{
    if (isLldbActive)
    {
        vcSignal(sem);
        // The simulation controller should ensure this executes uninterrupted
        // but the other usages might need attention
        sem->count = sem->count + 1;
        return;
    }
    platform_semSignal(sem);
}

void platform_semSignal(CSSem* sem)
{
    // Platform-dependent senaphore release.
    #ifdef _WIN32 // Windows version
        if(!ReleaseSemaphore(sem->sem, 1, NULL))
            vizconError("vcSemSignal/vcSemSignalMult", GetLastError());
        sem->count = sem->count + 1;
    #elif __linux__ || __APPLE__ // POSIX version
        if(sem_post(sem->sem))
            vizconError("vcSemSignal/vcSemSignalMult", errno);
        sem->count = sem->count + 1;
    #endif
}

// semClose - Close the semaphore and free the associated struct.
void semClose(CSSem* sem)
{
    if (isLldbActive)
    {
        fprintf(stderr, "Warning: semClose is unimplemented!\n"); 
        return;
    }
    // Platform-dependent closure and memory management.
    #ifdef _WIN32 // Windows version.
        if(!CloseHandle(sem->sem))
        {
            // FIXME: Change referenced function.
            vizconError("vcSemClose", GetLastError());
        }
        free(sem);
    #elif __linux__ || __APPLE__ // POSIX version.
        if(sem_close(sem->sem))
        {
            // FIXME: Change referenced function.
            vizconError("vcSemClose", errno);
        }
        free(sem);
    #endif
}