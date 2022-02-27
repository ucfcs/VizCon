#include "semaphores.h"
#include <pthread.h>
#include "threads.h"

void vcJoin(CSThread *thread, void *ret);
void doCreateThread(CSThread *thethread, void *thefunc(void *), void *theparam);
int lldb_printf(const char *format_string, ...);

void vc_internal_registerSem(CSSem *sem); 
void vcWait(CSSem *sem);
void vcSignal(CSSem *sem);