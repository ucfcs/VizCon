#pragma once

// Utilities library
#include "utils.h"

// Platform-dependent definitions.
#ifdef _WIN32 // Windows version
    #define MUTEX_TYPE HANDLE
    #define THREAD_ID_TYPE DWORD
#elif __linux__ // Linux version
    #define MUTEX_TYPE pthread_mutex_t*
    #define THREAD_ID_TYPE pthread_t
#elif __APPLE__ // Apple version
    #define MUTEX_TYPE pthread_mutex_t*
    // TODO: according to the man page for pthread_threadid_np
    // the return type of that func is an int, which means this must also be an int
    #define THREAD_ID_TYPE int
#endif

// CSMutex - A wrapper for the system's mutex type.
typedef struct CSMutex
{
    MUTEX_TYPE mutex;        // Mutex object
    int available;           // Flags whether the mutex is currently available.
    THREAD_ID_TYPE holderID; // If locked, ID of thread that placed the lock.
    struct CSMutex* next;    // The next mutex in the list.
} CSMutex;

// Function prototypes.
CSMutex* mutexCreate(); // Creates a mutex.
void mutexLock(CSMutex* mutex);   // Waits for mutex availability, then locks.
int mutexTryLock(CSMutex* mutex); // Attempts to lock without waiting.
void mutexUnlock(CSMutex* mutex); // Unlocks the mutex.
int mutexStatus(CSMutex* mutex);  // Returns whether the mutex is available.
void mutexClose(CSMutex* mutex);  // Closes and destroys the mutex struct.
