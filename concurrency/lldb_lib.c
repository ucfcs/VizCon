#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include "lldb_lib.h"

int real_main(void);
sem_t sem_wait_create_thread;
int isLldbActive; 

void do_post(void) {
	sem_post(&sem_wait_create_thread);
}

void *vc_internal_thread_wrapper(void *parameter) {
	CSThread *thread = parameter;
	//printf("Thread runs!\n");
	do_post();
	void *retval = thread->func(thread->arg);
	return retval;
}
void lldb_hook_createThread(CSThread *thread) {
	// LLDB
}

void vcJoin(CSThread *thread, void *ret) {
	// pthread_join(thread, &ret);
}

void vc_internal_init() {
	char *lldbMode = getenv("lldbMode");
	isLldbActive = lldbMode != NULL && strcmp(lldbMode, "1") == 0;
	if (isLldbActive) {
		fprintf(stderr, "LLDB is active\n");
	}
	else {
		fprintf(stderr, "LLDB is NOT active\n");
	}
	sem_init(&sem_wait_create_thread, 0, 0);
}



void vc_internal_registerSem(CSSem *sem) {

}

void vcWait(CSSem *sem) {

}

void vcSignal(CSSem *sem) {

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