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
    char* name;         // Internal name.
    int num;            // Internal identifier.
    SEM_VALUE count;    // The semaphore's current value.
    struct CSSem* next; // The next semaphore in the global list.
} CSSem;

// Function prototypes
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue); // Creates a semaphore.
void semWait(CSSem* sem);                            // Waits for a permit.
int semTryWait(CSSem* sem);                          // Tries to get a permit without waiting.
void semSignal(CSSem* sem);                          // Signals the semaphore.
void semClose(CSSem* sem);                           // Closes the semaphore and frees memory.