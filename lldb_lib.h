void vcJoin(pthread_t thread, void *ret);
void doCreateThread(pthread_t *thethread, void *thefunc(void *), void *theparam);

struct vcSem {

};
struct vcSem *vcCreateSemaphore(void);
void vcWait(struct vcSem *sem);
void vcSignal(struct vcSem *sem);