#ifndef CONCURRENCYLIB_H
#define CONCURRENCYLIB_H
//universal libraries
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32 // window's libraries and definitions
#include <windows.h>
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID
#define SEM_TYPE HANDLE
#define SEM_NAME LPCSTR
#define SEM_VALUE LONG
#define MUTEX_TYPE HANDLE
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*
#define SEM_TYPE sem_t*
#define SEM_NAME const char*
#define SEM_VALUE unsigned int
#define MUTEX_TYPE pthread_mutex_t*
#endif

// meta: proto type for the compiled name of the user function so we have access to it in our program
int userMain(void); 

// concurrency simulator threading structure
typedef struct CSThread {
    #if defined(_WIN32) // windows
    HANDLE thread;
    THREAD_RET id;
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_t thread;
    #endif
    THREAD_RET returnVal;
    struct CSThread* next;
} CSThread;

// concurrency simulator semaphore structure
typedef struct CSSem {
    SEM_TYPE sem;
    SEM_VALUE count;
    struct CSSem* next;
} CSSem;

// concurrency simulator mutex structure
typedef struct CSMutex {
    MUTEX_TYPE mutex;
    int available;
    struct CSMutex* next;
    // FIXME: int holderID; // ID of thread that locked the mutex, if it's locked.
} CSMutex;

//Thread functions
CSThread* cobeginThread(void* arg);
THREAD_RET createThread(void* arg);
void joinThread(CSThread* thread);
void freeCSThread(CSThread* thread);
void sleepThread(int milliseconds);

//Semaphore functions
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue);
void semSignal(CSSem* sem);
void semWait(CSSem* sem);
int semTryWait(CSSem* sem);
int semValue(CSSem* sem);
void semClose(CSSem* sem);

//Mutex functions
CSMutex* mutexCreate(char* name);
void mutexLock(CSMutex* mutex);
int mutexTryLock(CSMutex* mutex);
void mutexUnlock(CSMutex* mutex);
void mutexClose(CSMutex* mutex);
int mutexStatus(CSMutex* mutex);

//Other functions
void vizconAbort();
void vizconError(int func, int err);

#endif