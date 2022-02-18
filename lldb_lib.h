void vcJoin(pthread_t thread, void *ret);
void doCreateThread(pthread_t *thethread, void *thefunc(void *), void *theparam);
int lldb_printf(const char *format_string, ...);
#define printf lldb_printf
struct vcSem {

};
struct vcSem *vcCreateSemaphore(void);
void vcWait(struct vcSem *sem);
void vcSignal(struct vcSem *sem);