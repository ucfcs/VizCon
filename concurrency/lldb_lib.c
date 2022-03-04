#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lldb_lib.h"

int real_main(void);
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
    return retval;
}
void lldb_hook_createThread(CSThread *thread)
{
    // LLDB
}

void vcJoin(CSThread *thread, void *ret)
{
    // pthread_join(thread, &ret);
}

void vc_internal_init()
{
    char *lldbMode = getenv("lldbMode");
    isLldbActive = lldbMode != NULL && strcmp(lldbMode, "1") == 0;
    if (isLldbActive)
    {
        setbuf(stdout, NULL);
        fprintf(stderr, "LLDB is active\n");
    }
    else
    {
        fprintf(stderr, "LLDB is NOT active\n");
    }

    if (isLldbActive)
    {
        isLldbActive = 0; // Global state is bad
        sem_wait_create_thread = semCreate("sem_wait_create_thread", 1);
        platform_semWait(sem_wait_create_thread);
        isLldbActive = 1;
    }
}



void vc_internal_registerSem(CSSem *sem, char *name, int initialValue, int maxValue)
{

}

void vcWait(CSSem *sem)
{

}

void vcSignal(CSSem *sem)
{

}

int main(void)
{
    vc_internal_init();
    return real_main();
}
