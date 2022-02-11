#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

#define main userMain

// Lists used to track all threads and semaphores
CSThread* vizconThreadList;
CSThread* vizconThreadListInitial;

// Thread functions
void vcThreadQueue(threadFunc func, void *arg);
void vcThreadStart();
THREAD_RET *vcThreadReturn();

// Alternate function names.
#define vcMutexLockCreate vcMutexCreate
#define vcLock vcMutexLock
#define vcUnlock vcMutexUnlock
#define vcLockAvailable vcMutexStatus

// VCMutex - A node for a mutex in the global mutex list.
typedef struct VCMutex
{
    CSMutex* mutex;       // The corresponding mutex.
    struct VCMutex* next; // The next mutex in the list.
} VCMutex;

// Mutex functions
VCMutex* vcMutexCreate(char* name); // Creates a VCMutex with the given name.
void vcMutexLock(VCMutex* mutex);   // Waits for availability, then locks.
int vcMutexTrylock(VCMutex* mutex); // Attempts to lock without waiting.
void vcMutexUnlock(VCMutex* mutex); // Unlocks mutex.
int vcMutexStatus(VCMutex* mutex);  // Returns whether the mutex is available.
