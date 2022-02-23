#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "lldb_lib.h"

struct vcSem *counter_sem;
int counter;

void *thread_function(void *parameter)
{
	int *threadid = parameter;
	for (int i = 0; i < 10; i++) {
		//vcWait(counter_sem);
		//counter++;
		int local = counter;
		counter = local + 1;
		//vcSignal(counter_sem);
	}
	return NULL;
}

int real_main(void)
{
	printf("Hello world!\n");
	counter_sem = vcCreateSemaphore();

	pthread_t thread1;
	pthread_t thread2;
	void *ret_from_thread1;
	void *ret_from_thread2;

	counter = 0;
	int thread1id = 1;
	int thread2id = 2;

	printf("Before create\n");
	doCreateThread(&thread1, thread_function, &thread1id);
	printf("After create #1\n");
	doCreateThread(&thread2, thread_function, &thread2id);
	printf("After create #2\n");

	vcJoin(thread1, &ret_from_thread1);
	vcJoin(thread2, &ret_from_thread2);
	printf("Done. Counter is %d\n", counter);
	return 0;
}
