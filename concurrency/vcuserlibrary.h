#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

//#define main userMain

// Thread functions
void vcThreadQueue(threadFunc func, void *arg);
void vcThreadQueueNamed(threadFunc func, void *arg, char *name);
void vcThreadStart();
void** vcThreadReturn();

// Alternate type names.
#define vcSem CSSem*
#define vcSemaphore CSSem*
#define vcMutex CSMutex*

// Alternate funtion names.
#define vcAcquire vcSemWait
#define vcAcquireMult vcSemWaitMult
#define vcP vcSemWait
#define vcPMult vcSemWaitMult
#define vcRelease vcSemSignal
#define vcReleaseMult vcSemSignalMult
#define vcV vcSemSignal
#define vcVMult vcSemSignalMult
#define vcMutexLockCreate vcMutexCreate
#define vcMutexLockCreateNamed vcMutexCreateNamed
#define vcLock vcMutexLock
#define vcUnlock vcMutexUnlock
#define vcLockAvailable vcMutexStatus

//Semaphore functions
vcSem vcSemCreate(int count);
vcSem vcSemCreateNamed(int count, char* name);
void vcSemWait(vcSem sem);
void vcSemWaitMult(vcSem sem, int num);
int vcSemTryWait(vcSem sem);
int vcSemTryWaitMult(vcSem sem, int num);
void vcSemSignal(vcSem sem);
void vcSemSignalMult(vcSem sem, int num);
int vcSemValue(vcSem sem);

// Mutex functions
vcMutex vcMutexCreate();                // Creates a VCMutex.
vcMutex vcMutexCreateNamed(char* name); // Creates a VCMutex with the name.
void vcMutexLock(vcMutex mutex);        // Waits for availability, then locks.
int vcMutexTrylock(vcMutex mutex);      // Attempts to lock without waiting.
void vcMutexUnlock(vcMutex mutex);      // Unlocks mutex.
int vcMutexStatus(vcMutex mutex);       // Returns whether mutex is available.