// Universal Libraries
#include "utils.h"

// Platform dependent libraries
#ifdef _WIN32
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID

typedef struct CSThread
{
    char *name;
    int num;
    HANDLE thread;
    THREAD_RET id;
    THREAD_RET returnVal;
    struct CSThread *next;
} CSThread;

#elif __linux__ || __APPLE__
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*

typedef struct CSThread
{
    char *name;
    int num;
    pthread_t thread;
	threadFunc func;
	void *arg;
    THREAD_RET returnVal;
    struct CSThread *next;
} CSThread;

#endif

// Thread Function Prototypes
CSThread* createThread(threadFunc func, void *arg);
void joinThread(CSThread *thread);
void freeThread(CSThread *thread);
void startThread(CSThread* thread);