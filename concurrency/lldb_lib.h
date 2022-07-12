#include "semaphores.h"
#include "threads.h"
#include "mutexes.h"

void lldb_hook_joinThread(CSThread *thread);
void *vc_internal_thread_wrapper(void *parameter);
void lldb_hook_createThread(CSThread *thread, char *name);
void lldb_waitForThreadStart(void);
void lldb_hook_freeThread(CSThread *thread);
void lldb_hook_threadSleep(int milliseconds);

void lldb_hook_registerSem(CSSem *sem, int initialValue, int maxValue);
void lldb_hook_semWait(CSSem *sem);
int lldb_hook_semSignal(CSSem *sem);
int lldb_hook_semTryWait(CSSem *sem);
void lldb_hook_semClose(CSSem *sem);

void lldb_hook_registerMutex(CSMutex *mutex);
int lldb_hook_lockMutex(CSMutex *mutex);
int lldb_hook_unlockMutex(CSMutex *mutex);
int lldb_hook_mutexTryLock(CSMutex *mutex);
void lldb_hook_mutexClose(CSMutex *mutex);
