// Universal libraries
#include <stdio.h>
#include <stdlib.h>

// Utils library
#include "utils.h"

// Platform-dependent libraries
#ifdef _WIN32 // Windows version
#include <windows.h>
#define MUTEX_TYPE HANDLE
#define THREAD_ID_TYPE DWORD
#elif __linux__ || __APPLE__ // POSIX version
#include <pthread.h>
#include <errno.h>
#define MUTEX_TYPE pthread_mutex_t*
#define THREAD_ID_TYPE pthread_t
#endif

// Constant used for default name generation.
#define NAME_ID_MUTEX 2

// Constants used for VizCon error codes.
#define ERROR_MUTEX_ABANDONED 500
#define ERROR_MUTEX_TIMEOUT 501
#define ERROR_MUTEX_DOUBLE_UNLOCK 510
#define ERROR_MUTEX_DOUBLE_LOCK 511
#define ERROR_MUTEX_CROSS_THREAD_UNLOCK 512

// CSMutex - Wrapper for the system's mutex type.
typedef struct CSMutex
{
    MUTEX_TYPE mutex;        // Mutex object
    int available;           // Flags whether the mutex is currently available.
    THREAD_ID_TYPE holderID; // If locked, ID of thread that placed the lock.
} CSMutex;

// Mutex function prototypes.
// Function contents can be found in mutexes.c.
CSMutex* mutexCreate(char* name); // Creates a mutex with the given name.
void mutexLock(CSMutex* mutex);   // Waits for mutex availability, then locks.
int mutexTryLock(CSMutex* mutex); // Attempts to lock without waiting.
void mutexUnlock(CSMutex* mutex); // Unlocks mutex.
void mutexClose(CSMutex* mutex);  // Closes and destroys the mutex struct.
int mutexStatus(CSMutex* mutex);  // Returns whether the mutex is available.