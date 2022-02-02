#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

// VCMutex - Wrapper for the CSMutex type.
typedef struct VCMutex
{
    CSMutex* mutex;       // Mutex object
    struct VCMutex* next; // Next VCMutex struct in the mutex list
} VCMutex;

// Mutex functions
VCMutex* vcMutexCreate(char* name);
void vcMutexLock(VCMutex* mutex);
int vcMutexTrylock(VCMutex* mutex);
void vcMutexUnlock(VCMutex* mutex);
int vcMutexStatus(VCMutex* mutex);