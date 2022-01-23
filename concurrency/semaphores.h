#include "concurrency.h"

#ifdef _WIN32 // window's libraries and definitions
#define SEM_TYPE HANDLE
#define SEM_NAME LPCSTR
#define SEM_VALUE LONG
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
#include <semaphore.h>
#define SEM_TYPE sem_t*
#define SEM_NAME const char*
#define SEM_VALUE unsigned int
#endif

// concurrency simulator semaphore structure
typedef struct CSSem {
    SEM_TYPE sem;
    SEM_VALUE count;
    struct CSSem* next;
} CSSem;

//Semaphore functions
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue);
void semSignal(CSSem* sem);
void semWait(CSSem* sem);
int semTryWait(CSSem* sem);
int semValue(CSSem* sem);
void semClose(CSSem* sem);