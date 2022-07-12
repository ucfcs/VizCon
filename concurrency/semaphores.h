#pragma once
// Utilities library
#include "utils.h"

// Platform-dependent libraries and definitions.
#ifdef _WIN32
    #define SEM_TYPE HANDLE
    #define SEM_NAME LPCSTR
    #define SEM_VALUE LONG
#elif __linux__ || __APPLE__
    #include <semaphore.h>
    #include <fcntl.h>
    #define SEM_TYPE sem_t*
    #define SEM_NAME const char*
    #define SEM_VALUE unsigned int
#endif

// CSSem - A wrapper for the system's semaphore type.
typedef struct CSSem
{
    SEM_TYPE sem;       // Semaphore object.
    SEM_VALUE count;    // The semaphore's current value.
    SEM_VALUE maxCount; // The semaphore's maximum value.
    struct CSSem* next; // The next semaphore in the global list.
} CSSem;

// Function prototypes
CSSem* semCreate(SEM_VALUE maxValue);                // Creates a semaphore.
void semWait(CSSem* sem);                            // Waits for a permit.
int semTryWait(CSSem* sem);                          // Tries to get a permit without waiting.
void semSignal(CSSem* sem);                          // Signals the semaphore.
void semClose(CSSem* sem);                           // Closes the semaphore and frees memory.

CSSem* allocSem(SEM_VALUE maxValue);
CSSem* platform_semCreate(SEM_VALUE maxValue);
CSSem* lldb_semCreate(SEM_VALUE maxValue);
void platform_semSignal(CSSem* sem);
void platform_semWait(CSSem* sem);
