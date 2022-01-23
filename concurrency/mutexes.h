// Universal libraries
#include <stdio.h>
#include <stdlib.h>

// Platform-dependent libraries
#ifdef _WIN32 // Windows version
#include <windows.h>
#define MUTEX_TYPE HANDLE
#elif __linux__ || __APPLE__ // POSIX version
#include <pthread.h>
#include <errno.h>
#define MUTEX_TYPE pthread_mutex_t*
#endif

// Constants used for VizCon function codes.
#define FUNC_MUTEX_CREATE 8
#define FUNC_MUTEX_LOCK 9
#define FUNC_MUTEX_TRYLOCK 10
#define FUNC_MUTEX_UNLOCK 11
#define FUNC_MUTEX_STATUS 12
#define FUNC_MUTEX_CLOSE 13

// Constants used for VizCon error codes.
#define ERROR_MUTEX_ABANDONED 500
#define ERROR_MUTEX_TIMEOUT 501
#define ERROR_MUTEX_DOUBLE_UNLOCK 502
#define ERROR_MUTEX_DOUBLE_LOCK 503
#define ERROR_MUTEX_CROSS_THREAD_UNLOCK 504

// VCMutex - Wrapper for the system's mutex type.
typedef struct VCMutex
{
    MUTEX_TYPE mutex;     // Mutex object
    int available;        // Flag for whether the mutex is currently available.
    int holderID;         // If locked, ID of thread that placed the lock.
    struct VCMutex* next; // Next VCMutex struct in the mutex list
} VCMutex;

// Mutex function prototypes.
// Function contents can be found in mutexes.c.
VCMutex* mutexCreate(char* name); // Creates a mutex with the given name.
void mutexLock(VCMutex* mutex);   // Waits for mutex availability, then locks.
int mutexTryLock(VCMutex* mutex); // Attempts to lock without waiting.
void mutexUnlock(VCMutex* mutex); // Unlocks mutex.
void mutexClose(VCMutex* mutex);  // Closes and destroys the mutex struct.
int mutexStatus(VCMutex* mutex);  // Returns whether the mutex is available.