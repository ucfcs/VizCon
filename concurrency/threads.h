// Utilities library
#include "utils.h"

// Windows libraries and definitions
#ifdef _WIN32
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID

// CSThread - A wrapper for the Windows thread handle.
//            POSIX systems use a different version.
typedef struct CSThread
{
    char *name;            // Internal name.
    int num;               // Internal Identifier.
    HANDLE thread;         // The handle which represents the thread.
    THREAD_RET returnVal;  // A location for the return value.
    struct CSThread *next; // The next thread in the global list.
} CSThread;

// POSIX libraries and definitions
#elif __linux__ || __APPLE__
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*

// CSThread - A wrapper for the POSIX thread type.
//            Windows systems use a different version.
typedef struct CSThread
{
    char *name;            // Internal name.
    int num;               // Internal identifier.
    pthread_t thread;      // The variable which represents the thread.
    threadFunc func;       // The function the thread starts with.
    void *arg;             // The argument(s) the thread starts with.
    THREAD_RET returnVal;  // A location for the return value.
    struct CSThread *next; // The next thread in the global list.
} CSThread;

#endif

// Function prototypes
CSThread* createThread(threadFunc func, void *arg);
void joinThread(CSThread *thread);
void freeThread(CSThread *thread);
void startThread(CSThread* thread);