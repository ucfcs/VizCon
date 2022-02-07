#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

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