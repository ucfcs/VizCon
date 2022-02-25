// Utilities library
#include "utils.h"

// Windows libraries and definitions
#ifdef _WIN32
    #define SEM_TYPE HANDLE
    #define SEM_NAME LPCSTR
    #define SEM_VALUE LONG

// POSIX libraries and definitions
#elif __linux__ || __APPLE__
    #include <semaphore.h>
    #include <fcntl.h>
    #define SEM_TYPE sem_t*
    #define SEM_NAME const char*
    #define SEM_VALUE unsigned int
#endif

// CSSem - A wrapper for the system's semaphore type.
typedef struct CSSem {
    SEM_TYPE sem;       // Semaphore object.
    char* name;         // Internal name.
    int num;            // Internal identifier.
    SEM_VALUE count;    // The semaphore's current value.
    struct CSSem* next; // The next semaphore in the global list.
} CSSem;

// Function prototypes.
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue);
void semSignal(CSSem* sem);
void semWait(CSSem* sem);
int semTryWait(CSSem* sem);
int semValue(CSSem* sem);
void semClose(CSSem* sem);