#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "lldb_lib.h"

int real_main(void);
sem_t sem_wait_create_thread;

void do_post(pthread_t thread) {
	sem_post(&sem_wait_create_thread);
}
struct vc_internal_wrapped_thread {
	void *parameter;
	void *(*thefunc)(void *);
	pthread_t thread;
};
void *vc_internal_thread_wrapper(void *parameter) {
	struct vc_internal_wrapped_thread *threadinfo = parameter;
	//printf("Thread runs!\n");
	do_post(pthread_self());
	void *retval = threadinfo->thefunc(threadinfo->parameter);
	free(threadinfo);
	return retval;
}
void doCreateThread(pthread_t *thethread, void *thefunc(void *), void *theparam)
{
	struct vc_internal_wrapped_thread *threadinfo = malloc(sizeof(struct vc_internal_wrapped_thread));
	threadinfo->parameter = theparam;
	threadinfo->thefunc = thefunc;
	pthread_create(thethread, NULL, vc_internal_thread_wrapper, threadinfo);
	sem_wait(&sem_wait_create_thread);
	//printf("doCreateThread is over\n");
}
void vcJoin(pthread_t thread, void *ret) {
	// pthread_join(thread, &ret);
}

void vc_internal_init() {
	sem_init(&sem_wait_create_thread, 0, 0);
}



void vc_internal_registerSem(struct vcSem *sem) {

}
struct vcSem *vcCreateSemaphore()
{
	// TODO: ?
	struct vcSem *sem = malloc(sizeof(struct vcSem));
	vc_internal_registerSem(sem);
	return sem;
}

void vcWait(struct vcSem *sem) {

}

void vcSignal(struct vcSem *sem) {

}

int main(void)
{
	vc_internal_init();
	return real_main();
}

void printf_hook(const char* str) {
}
// From 

int lldb_printf(const char *format_string, ...)
{
	va_list args;
	va_start(args, format_string);
	char buf[1024];
	int ret = vsnprintf(buf, 1024, format_string, args);
	printf_hook(buf);
	va_end(args);
	return ret;
}