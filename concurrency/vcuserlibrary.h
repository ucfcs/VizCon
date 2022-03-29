// Concurrency libraries
#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

// Redefinition of the user's main function.
// May be needed to start integration with LLDB.
//#define main userMain

// User type definitions
#define vcSem CSSem*
#define vcSemaphore CSSem*
#define vcMutex CSMutex*

// Alternate function names
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

// User thread functions
void vcThreadQueue(threadFunc func, void *arg);                  // Queues a thread.
void vcThreadQueueNamed(threadFunc func, void *arg, char *name); // Queues a thread with the name.
void vcThreadStart();                                            // Starts all threads.
void** vcThreadReturn();                                         // Starts all threads and returns their values.
void vcThreadSleep(int milliseconds);                            //Puts the calling thread to sleep for specified amount of time in milliseconds.
int vcThreadId();                                                //returns the id of the calling thread.

// User semaphore functions
vcSem vcSemCreate(int maxCount);                                           // Creates a semaphore with a user-specified max value.
vcSem vcSemCreateInitial(int maxCount, int initialCount);                  // Creates a semaphore with a user-specified max and initial value.
void vcSemWait(vcSem sem);                                                 // Waits for availability, then takes one permit.
void vcSemWaitMult(vcSem sem, int num);                                    // Waits for availability, then takes multiple permits.
int vcSemTryWait(vcSem sem);                                               // Attempts to take one permit without waiting.
int vcSemTryWaitMult(vcSem sem, int num);                                  // Attempts to take multiple permits without waiting.
void vcSemSignal(vcSem sem);                                               // Returns one permit.
void vcSemSignalMult(vcSem sem, int num);                                  // Returns multiple permits.
int vcSemValue(vcSem sem);                                                 // Returns the number of available permits.

// User mutex functions
vcMutex vcMutexCreate();                // Creates a mutex.
void vcMutexLock(vcMutex mutex);        // Waits for availability, then locks.
int vcMutexTrylock(vcMutex mutex);      // Attempts to lock without waiting.
void vcMutexUnlock(vcMutex mutex);      // Unlocks the mutex.
int vcMutexStatus(vcMutex mutex);       // Returns whether mutex is available.

// Other user functions
void vcHalt(int exitCode); // Immediately frees all concurrency objects and exits.