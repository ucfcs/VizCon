#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lldb_lib.h"

extern int userMain();
CSSem *sem_wait_create_thread;
int isLldbActive; 

void do_post(void)
{
    platform_semSignal(sem_wait_create_thread);
}
void lldb_waitForThreadStart(void)
{
    platform_semWait(sem_wait_create_thread);
}
void *vc_internal_thread_wrapper(void *parameter)
{
    CSThread *thread = parameter;
    //printf("Thread runs!\n");
    do_post();
    void *retval = thread->func(thread->arg);
    thread->returnVal = retval;
    return retval;
}
void lldb_hook_createThread(CSThread *thread, char *name)
{
    // LLDB
}

void vcJoin(CSThread *thread, void *ret)
{
    // pthread_join(thread, &ret);
}

void lldb_hook_freeThread(CSThread *thread)
{
    // LLDB
}

void lldb_hook_threadSleep(int milliseconds)
{
    // LLDB
}

void vc_internal_init()
{
    char *lldbMode = getenv("lldbMode");
    isLldbActive = lldbMode != NULL && strcmp(lldbMode, "1") == 0;
    if (isLldbActive)
    {
        setbuf(stdout, NULL);
        //fprintf(stderr, "LLDB is active\n");
    }
    else
    {
        //fprintf(stderr, "LLDB is NOT active\n");
    }

    if (isLldbActive)
    {
        isLldbActive = 0; // Global state is bad
        sem_wait_create_thread = semCreate(1);
        platform_semWait(sem_wait_create_thread);
        isLldbActive = 1;
    }
}


// Semaphores
void vc_internal_registerSem(CSSem *sem, int initialValue, int maxValue)
{
    // LLDB
}

void vcWait(CSSem *sem)
{
    // LLDB
}

void vcSignal(CSSem *sem)
{
    // LLDB
}

int lldb_hook_semTryWait(CSSem *sem)
{
    int res;
    // LLDB
    fprintf(stderr, "Error\n");
    res = -9;
    return res;
}

void lldb_hook_semClose(CSSem *sem)
{
    // LLDB
}

// Mutexes
void lldb_hook_registerMutex(CSMutex *mutex)
{
    // LLDB
}

void lldb_hook_lockMutex(CSMutex *mutex)
{
    // LLDB
}

void lldb_hook_unlockMutex(CSMutex *mutex)
{
    // LLDB
}

int lldb_hook_mutexTryLock(CSMutex *mutex)
{
    int res;
    // LLDB
    fprintf(stderr, "Error\n");
    res = -9;
    return res;
}

void lldb_hook_mutexClose(CSMutex *mutex)
{
    // LLDB
}

int main()
{
    vc_internal_init();
    return userMain();
}
