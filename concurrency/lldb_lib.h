#include "semaphores.h"
#include "threads.h"

void vcJoin(CSThread *thread, void *ret);
//void doCreateThread(CSThread *thethread, void *thefunc(void *), void *theparam);
void *vc_internal_thread_wrapper(void *parameter);
void lldb_hook_createThread(CSThread *thread, char *name);
void lldb_waitForThreadStart(void);

void vc_internal_registerSem(CSSem *sem, char *name, int initialValue, int maxValue);
void vcWait(CSSem *sem);
void vcSignal(CSSem *sem);