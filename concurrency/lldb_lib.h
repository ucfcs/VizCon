#include "semaphores.h"
#include "threads.h"
#include "mutexes.h"

void vcJoin(CSThread *thread, void *ret);
//void doCreateThread(CSThread *thethread, void *thefunc(void *), void *theparam);
void *vc_internal_thread_wrapper(void *parameter);
void lldb_hook_createThread(CSThread *thread, char *name);
void lldb_waitForThreadStart(void);

void vc_internal_registerSem(CSSem *sem, int initialValue, int maxValue);
void vcWait(CSSem *sem);
void vcSignal(CSSem *sem);

void lldb_hook_registerMutex(CSMutex *mutex);
void lldb_hook_lockMutex(CSMutex *mutex);
void lldb_hook_unlockMutex(CSMutex *mutex);
