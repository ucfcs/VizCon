#include "semaphores.h"
#include "threads.h"

void vcJoin(CSThread *thread, void *ret);
//void doCreateThread(CSThread *thethread, void *thefunc(void *), void *theparam);
void *vc_internal_thread_wrapper(void *parameter);
int lldb_printf(const char *format_string, ...);
void lldb_hook_createThread(CSThread *thread);
void lldb_waitForThreadStart(void);

void vc_internal_registerSem(CSSem *sem); 
void vcWait(CSSem *sem);
void vcSignal(CSSem *sem);