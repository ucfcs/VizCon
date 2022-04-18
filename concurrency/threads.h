#pragma once
// Utilities library
#include "utils.h"

// Thread type definition
#ifdef _WIN32
    #define THREAD_TYPE HANDLE
#elif __linux__ || __APPLE__
    #define THREAD_TYPE pthread_t
    #include <unistd.h>
    #include <sys/syscall.h>
    #define gettid() syscall(SYS_gettid)
#endif

// Thread function definition
typedef void* (*threadFunc) (void* param);

// CSThread - A wrapper for the thread type.
typedef struct CSThread
{
    char *name;            // Internal name.
    int num;               // Internal identifier.
    THREAD_TYPE thread;    // The variable which represents the thread.
    threadFunc func;       // The function the thread starts with.
    void *arg;             // The argument(s) the thread starts with.
    void* returnVal;       // A location for the return value.
    struct CSThread *next; // The next thread in the global list.
} CSThread;

// Function prototypes
CSThread* createThread(threadFunc func, void *arg); // Creates a thread.
void startThread(CSThread* thread);                 // Starts the thread.
void joinThread(CSThread *thread);                  // Waits for a finish, then saves the return value.
void freeThread(CSThread *thread);                  // Closes the thread and frees memory.