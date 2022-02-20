// Utils library
#include "utils.h"

// Platform-dependent libraries
#ifdef _WIN32 // Windows version
#define MUTEX_TYPE HANDLE
#define THREAD_ID_TYPE DWORD
#elif __linux__ || __APPLE__ // POSIX version
#define MUTEX_TYPE pthread_mutex_t*
#define THREAD_ID_TYPE pthread_t
#endif

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