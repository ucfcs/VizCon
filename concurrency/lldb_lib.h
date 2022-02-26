#include "semaphores.h"
#include <pthread.h>
void vcJoin(pthread_t thread, void *ret);
void doCreateThread(pthread_t *thethread, void *thefunc(void *), void *theparam);
int lldb_printf(const char *format_string, ...);

void vc_internal_registerSem(CSSem *sem); 
void vcWait(CSSem *sem);
void vcSignal(CSSem *sem);