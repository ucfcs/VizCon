#ifndef VCUSERLIBRARY_H
#define VCUSERLIBRARY_H
#include "concurrencylib.h"

#define vcSem CSSem
#define vcMutex CSMutex

//Thread functions
void vcCobegin(threadFunc func, void* arg);
void vcWaitForCompletion();
THREAD_RET* vcWaitForReturn();

//Semaphore functions
vcSem* vcSemCreate(char* name, int count);
void vcSemWait(vcSem* sem);
int vcSemTryWait(vcSem* sem);
void vcSemSignal(vcSem* sem);
int vcSemValue(vcSem* sem);

//Mutex functions
vcMutex* vcMutexCreate(char* name);
void vcMutexLock(vcMutex* mutex);
int vcMutexTrylock(vcMutex* mutex);
void vcMutexUnlock(vcMutex* mutex);
int vcMutexStatus(vcMutex* mutex);

#endif