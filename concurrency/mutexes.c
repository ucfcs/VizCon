#include "mutexes.h"

// Tracks the number of mutexes created without names.
// This is used to generate internal names when needed.
int genericNames = 0;

// mutexCreate - Create a mutex struct with the given name.
//               Returns: a pointer to the mutex struct.
CSMutex* mutexCreate(char* name)
{
    // A name is required, so make one if the user doesn't provide it.
    if(name == NULL)
    {
        name = vizconCreateName(NAME_ID_MUTEX, genericNames + 1);
        genericNames++;
    }

    // Attempt to allocate the struct. Error out on failure.
    CSMutex* mutex = (CSMutex*) malloc(sizeof(CSMutex));
    if (mutex == NULL)
    {
        // Platform-dependent error code retrieval.
        #ifdef _WIN32 // Windows version
        int err = GetLastError();

        #elif __linux__ || __APPLE__ // POSIX version
        int err = errno;
        
        #endif

        free(mutex);
        vizconError("vcMutexCreate", err);
        return NULL;
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
        vizconError("vcMutexCreate", err);
        return NULL;
    }

    #elif __linux__ || __APPLE__ // POSIX version
    mutex->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(mutex->mutex, NULL))
    {
        int err = errno;
        free(mutex);
        vizconError("vcMutexCreate", err);
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
        vizconError("vcMutexLock", ERROR_MUTEX_DOUBLE_LOCK);
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
            vizconError("vcMutexLock", ERROR_MUTEX_ABANDONED);
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
            vizconError("vcMutexLock", ERROR_MUTEX_TIMEOUT);
    }

    #elif __linux__ || __APPLE__ // POSIX version
    if(mutex->holderID == pthread_self())
    {
        vizconError("vcMutexLock", ERROR_MUTEX_DOUBLE_LOCK);
        return;
    }

    // pthread_mutex_lock returns 0 on success.
    // With success, mark lock as unavailable.
    if(!pthread_mutex_lock(mutex->mutex))
    {
        mutex->available = 0;
        mutex->holderID = pthread_self();
    }
    else
        vizconError("vcMutexLock", errno);
    
    #endif
}

// mutexTryLock - Try to obtain the mutex.
//                If it's unavailable, return without waiting.
//                Returns: 1 if lock is available, 0 otherwise.
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
            vizconError("vcMutexTrylock", ERROR_MUTEX_ABANDONED);
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
            vizconError("vcMutexTrylock", errno);
            return 0;
        }
    }

    #endif

    return 0; // Supress "potential non-return" compiler warning.
}

// mutexUnlock - Release a mutex lock.
void mutexUnlock(CSMutex* mutex)
{
    // Check whether the lock is already unlocked. If so, error out.
    if(mutexStatus(mutex))
    {
        vizconError("vcMutexUnlock", ERROR_MUTEX_DOUBLE_UNLOCK);
        return;
    }
        
    // Platform-dependent mutex unlocking.
    // Check whether the lock was placed by the thread trying to unlock it.
    // If so, create a release request. Error out where needed.
    #ifdef _WIN32 // Windows version
    if(mutex->holderID != GetCurrentThreadId())
    {
        vizconError("vcMutexUnlock", ERROR_MUTEX_CROSS_THREAD_UNLOCK);
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
        vizconError("vcMutexUnlock", ERROR_MUTEX_CROSS_THREAD_UNLOCK);
        return;
    }

    if(pthread_mutex_unlock(mutex->mutex))
        vizconError("vcMutexUnlock", errno);

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
    // Forcibly unlock to ensure the destruction works.
    // This won't cause an error since the underlying mutex is unsecured.
    // This is not needed in Windows, which tracks abandonment by threads.
    if(pthread_mutex_unlock(mutex->mutex))
        vizconError("vcThreadStart/vcThreadReturn", errno);

    if(pthread_mutex_destroy(mutex->mutex))
        vizconError("vcThreadStart/vcThreadReturn", errno);
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