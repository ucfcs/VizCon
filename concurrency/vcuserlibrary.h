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

// Alternate type and function names.
#define vcSemaphore CSSem
#define vcMutex CSMutex
#define vcMutexLockCreate vcMutexCreate
#define vcLock vcMutexLock
#define vcUnlock vcMutexUnlock
#define vcLockAvailable vcMutexStatus

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
vcMutex* vcMutexCreate();                // Creates a VCMutex.
vcMutex* vcMutexCreateNamed(char* name); // Creates a VCMutex with the given name.
void vcMutexLock(vcMutex* mutex);        // Waits for availability, then locks.
int vcMutexTrylock(vcMutex* mutex);      // Attempts to lock without waiting.
void vcMutexUnlock(vcMutex* mutex);      // Unlocks mutex.
int vcMutexStatus(vcMutex* mutex);       // Returns whether the mutex is available.
