// Concurrency libraries
#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

// Redefinition of the user's main function.
// May be needed to start integration with LLDB.
int userMain();
#define main userMain

// User type definitions
#define vcSem CSSem*
#define vcSemaphore CSSem*
#define vcMutex CSMutex*

// Alternate function names
//Semaphore keyword init for create (linux)
#define vcSemInit vcSemCreate
#define vcSemInitInitial vcSemCreateInitial
//Semaphore keyword aquire for wait/trywait (java)
#define vcSemAcquire vcSemWait
#define vcSemAcquireMult vcSemWaitMult
#define vcSemTryAcquire vcSemTryWait
#define vcSemTryAcquireMult vcSemTryWaitMult
//Semaphore keyword P for wait/trywait (Dijkstra)
#define vcSemP vcSemWait
#define vcSemPMult vcSemWaitMult
#define vcSemTryP vcSemTryWait
#define vcSemTryPMult vcSemTryWaitMult
//Semaphore keyword post for signal (linux)
#define vcSemPost vcSemSignal
#define vcSemPostMult vcSemSignalMult
//Semaphore keyword release for signal (windows/java)
#define vcSemRelease vcSemSignal
#define vcSemReleaseMult vcSemSignalMult
//Semaphore keyword V for signal (Dijkstra)
#define vcSemV vcSemSignal
#define vcSemVMult vcSemSignalMult
//Semaphore keyword GetValue for value (linux)
#define vcSemGetValue vcSemValue
//Mutex keyword init for create (linux)
#define vcMutexInit vcMutexCreate
//Mutex keyword wait for lock/trylock (windows)
#define vcMutexWait vcMutexLock
#define vcMutexTryWait vcMutexTrylock
//Mutex keyword release for unlock (windows)
#define vcMutexRelease vcMutexUnlock
//Mutex keyword available for status
#define vcMutexAvailable vcMutexStatus

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