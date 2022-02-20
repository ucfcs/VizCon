#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

//#define main userMain
#define vcSem CSSem

// Thread functions
void vcThreadQueue(threadFunc func, void *arg);
void vcThreadStart();
THREAD_RET *vcThreadReturn();
void freeUserThreads();

// Alternate function names.
#define vcSemaphore CSSem
#define vcMutex VCMutex
#define vcMutexLockCreate vcMutexCreate
#define vcLock vcMutexLock
#define vcUnlock vcMutexUnlock
#define vcLockAvailable vcMutexStatus

// VCMutex - A node for a mutex in the global mutex list.
typedef struct VCMutex
{
    CSMutex* mutex;       // The corresponding mutex.
    char* name;           // Internal name.
    int num;              // Internal identifier.
    struct VCMutex* next; // The next mutex in the list.
} VCMutex;

//Semaphore functions
vcSem* vcSemCreate(int count);
vcSem* vcSemCreateNamed(int count, char* name);
void vcSemWait(vcSem* sem);
void vcSemWaitMult(vcSem* sem, int num);
int vcSemTryWait(vcSem* sem);
int vcSemTryWaitMult(vcSem* sem, int num);
void vcSemSignal(vcSem* sem);
void vcSemSignalMult(vcSem* sem, int num);
int vcSemValue(vcSem* sem);

// Mutex functions
VCMutex* vcMutexCreate();                // Creates a VCMutex.
VCMutex* vcMutexCreateNamed(char* name); // Creates a VCMutex with the given name.
void vcMutexLock(VCMutex* mutex);        // Waits for availability, then locks.
int vcMutexTrylock(VCMutex* mutex);      // Attempts to lock without waiting.
void vcMutexUnlock(VCMutex* mutex);      // Unlocks mutex.
int vcMutexStatus(VCMutex* mutex);       // Returns whether the mutex is available.
