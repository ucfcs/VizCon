// Universal Libraries
#include <stdio.h>
#include <stdlib.h>

// Platform dependent libraries
#ifdef _WIN32
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID

typedef struct VCThread
{
    HANDLE thread;
    THREAD_RET id;
    THREAD_RET returnVal;
    struct VCThread *next;
} VCThread;

#elif __linux__ || __APPLE__
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*

typedef struct VCThread
{
    pthread_t thread;
    THREAD_RET returnVal;
    struct VCThread *next;
} VCThread;

#endif

// Thread Function Prototypes
THREAD_RET createThread(void *arg);
void joinThread(VCThread *thread);
void freeVCThread(VCThread *thread);
void sleepThread (int milliseconds);