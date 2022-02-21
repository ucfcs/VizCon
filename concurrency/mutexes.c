#include "mutexes.h"

// mutexCreate - Create a mutex struct with the given name.
//               Returns: a pointer to the mutex struct.
CSMutex* mutexCreate(char* name)
{
    // vcMutexCreate and vcMutexCreateNamed should ensure the mutex is named.
    // If it somehow isn't, error out.
    if (name == NULL)
    {
        vizconError("vcMutexCreate/vcMutexCreateNamed", VC_ERROR_NAMEERROR);
        return NULL;
    }

    // Attempt to allocate the struct. Error out on failure.
    CSMutex* mutex = (CSMutex*) malloc(sizeof(CSMutex));
    if (mutex == NULL)
    {
        vizconError("vcMutexCreate/vcMutexCreateNamed", VC_ERROR_MEMORY);
        return 0;
    }

    // Set non-mutex properties.
    mutex->available = 1;
    mutex->holderID = (THREAD_ID_TYPE) 0;

    // Platform-dependent mutex creation.
    // Create a mutex with default settings. Error out where needed.
    #ifdef _WIN32 // Windows version
    mutex->mutex = CreateMutexA(NULL, FALSE, name);
    if(mutex->mutex == NULL)
    {
        int err = (int) GetLastError();
        free(mutex);
        vizconError("vcMutexCreate/vcMutexCreateNamed", err);
        return NULL;
    }

    #elif __linux__ || __APPLE__ // POSIX version
    mutex->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(mutex->mutex, NULL))
    {
        int err = errno;
        free(mutex);
        vizconError("vcMutexCreate/vcMutexCreateNamed", err);
        return NULL;
    }

    #endif

    return mutex;
}

// mutexLock - Obtain a lock on the mutex.
//             If a lock is not available yet, wait until it is.
void mutexLock(CSMutex* mutex)
{
    // Platform-dependent mutex locking.
    // Check whether the caller has obtained this lock already.
    // If not, create a non-timeout wait request. Error out where needed.
    #ifdef _WIN32 // Windows version
    if(mutex->holderID == GetCurrentThreadId())
    {
        vizconError("vcMutexLock", VC_ERROR_DOUBLELOCK);
        return;
    }

    DWORD ret = WaitForSingleObject(mutex->mutex, INFINITE);
    switch(ret)
    {
        // WAIT_OBJECT_0 - No error. Mark lock as unavailable.
        case WAIT_OBJECT_0:
        {
            mutex->available = 0;
            mutex->holderID = GetCurrentThreadId();
            break;
        }
        
        // WAIT_ABANDONED - Thread that locked the mutex died before unlock.
        //                  Print an error.
        case WAIT_ABANDONED:
        {
            vizconError("vcMutexLock", VC_ERROR_ABANDONED);
            break;
        }
        
        // WAIT_FAILED - OS-level error. Just pass it on.
        case WAIT_FAILED:
        {
            vizconError("vcMutexLock", GetLastError());
            break;
        }

        // WAIT_TIMEOUT - Mutex was not available before timeout.
        //                This shouldn't happen, but it's here for safety.
        case WAIT_TIMEOUT:
            vizconError("vcMutexLock", VC_ERROR_TIMEOUT);
    }

    #elif __linux__ || __APPLE__ // POSIX version
    if(mutex->holderID == pthread_self())
    {
        vizconError("vcMutexLock", VC_ERROR_DOUBLELOCK);
        return;
    }

    // pthread_mutex_lock returns 0 on success.
    // With success, mark lock as unavailable.
    int ret = pthread_mutex_lock(mutex->mutex);
    if(!ret)
    {
        mutex->available = 0;
        mutex->holderID = pthread_self();
    }
    else
    {
        vizconError("vcThreadStart/vcThreadReturn", ret);
        return;
    }
    
    #endif
}

// mutexTryLock - Try to obtain the mutex.
//                If it's unavailable, return without waiting.
//                Returns: 1 if lock was available, 0 otherwise.
int mutexTryLock(CSMutex* mutex)
{
    // Platform-dependent mutex lock trying.
    // Create a wait request with 0 timeout. Error out where needed.
    #ifdef _WIN32 // Windows version
    // Windows uses recursive locks, so a Windows thread can obtain a lock
    // on an already-locked mutex if it was the one that originally locked it.
    // To circumvent, immediately leave if the holder is the current thread.
    if (mutex->holderID == GetCurrentThreadId())
        return 0;

    DWORD ret = WaitForSingleObject(mutex->mutex, 0);
    switch(ret)
    {
        // WAIT_OBJECT_0 - No error. Mark lock as unavailable.
        case WAIT_OBJECT_0:
        {
            mutex->available = 0;
            mutex->holderID = GetCurrentThreadId();
            return 1;
        }

        // WAIT_TIMEOUT - Mutex was not available before timeout.
        //                Since timeout is 0, this means it's currently taken.
        case WAIT_TIMEOUT:
            return 0;
        
        // WAIT_ABANDONED - Thread that locked the mutex died before unlock.
        //                  Print an error.
        case WAIT_ABANDONED:
        {
            vizconError("vcMutexTrylock", VC_ERROR_ABANDONED);
            return 0;
        }
        
        // WAIT_FAILED - OS-level error. Just pass it on.
        case WAIT_FAILED:
        {
            vizconError("vcMutexTrylock", GetLastError());
            return 0;
        }
    }

    #elif __linux__ || __APPLE__ // POSIX version
    int ret = pthread_mutex_trylock(mutex->mutex);
    switch(ret)
    {
        // 0 - Success. Mark mutex as unavailable.
        case 0:
        {
            mutex->available = 0;
            mutex->holderID = pthread_self();
            return 1;
        }

        // EBUSY - Mutex is currently locked. Just leave.
        case EBUSY:
            return 0;

        // Default - OS-level error. Just pass it on.
        default:
        {
            vizconError("vcMutexTrylock", ret);
            return 0;
        }
    }

    #endif

    return 0; // Suppress "potential non-return" compiler warning.
}

// mutexUnlock - Release a mutex lock.
void mutexUnlock(CSMutex* mutex)
{
    // Check whether the lock is already unlocked. If so, error out.
    if(mutexStatus(mutex))
    {
        vizconError("vcMutexUnlock", VC_ERROR_DOUBLEUNLOCK);
        return;
    }
        
    // Platform-dependent mutex unlocking.
    // Check whether the lock was placed by the thread trying to unlock it.
    // If so, create a release request. Error out where needed.
    #ifdef _WIN32 // Windows version
    if(mutex->holderID != GetCurrentThreadId())
    {
        vizconError("vcMutexUnlock", VC_ERROR_CROSSTHREADUNLOCK);
        return;
    }

    // Mark the mutex as available.
    // This occurs before the release to correct a sequencing issue.
    // If there is an error, the action will be un-done.
    int oldHolderID = mutex->holderID;
    mutex->available = 1;
    mutex->holderID = (THREAD_ID_TYPE) 0;

    if(!ReleaseMutex(mutex->mutex))
    {
        mutex->available = 0;
        mutex->holderID = oldHolderID;
        vizconError("vcMutexUnlock", GetLastError());
    }

    #elif __linux__ || __APPLE__ // POSIX version
    if(mutex->holderID != pthread_self())
    {
        vizconError("vcMutexUnlock", VC_ERROR_CROSSTHREADUNLOCK);
        return;
    }

    int ret = pthread_mutex_unlock(mutex->mutex);
    if(ret)
    {
        vizconError("vcMutexUnlock", ret);
        return;
    }

    mutex->available = 1;
    mutex->holderID = (THREAD_ID_TYPE) 0;

    #endif
}

// mutexClose - Close the mutex lock and free the struct.
void mutexClose(CSMutex* mutex)
{
    // Platform-dependent mutex destruction.
    // Create a release request, then free the rest of the struct. 
    #ifdef _WIN32 // Windows version
    if(!CloseHandle(mutex->mutex))
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    // CloseHandle frees the THREAD object at mutex->mutex automatically.
    free(mutex);

    #elif __linux__ || __APPLE__ // POSIX version
    // Forcibly trylock and unlock to ensure the lock is free for destruction.
    // This won't cause an error since the underlying mutex is unsecured.
    // This is not needed in Windows, which tracks abandonment by threads.
    int ret = pthread_mutex_trylock(mutex->mutex);
    if(ret != 0 && ret != EBUSY)
    {
        vizconError("vcThreadStart/vcThreadReturn", ret);
        return;
    }

    ret = pthread_mutex_unlock(mutex->mutex);
    if(ret)
    {
        vizconError("vcThreadStart/vcThreadReturn", ret);
        return;
    }

    ret = pthread_mutex_destroy(mutex->mutex);
    if(ret)
    {
        vizconError("vcThreadStart/vcThreadReturn", ret);
        return;
    }
    
    // Free both the mutex and wrapper.
    free(mutex->mutex);
    free(mutex);

    #endif
}

// mutexStatus - Check whether the mutex is available.
//               Use the mutex->available value to avoid issues
//               with the recursive locks used by Windows.
//               Returns: 1 if lock is available, 0 otherwise.
int mutexStatus(CSMutex* mutex)
{
    return mutex->available;
}