#include "vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>

int loop = 100;
int counter;
vcSem counter_sem;

void* SimpleThread(void* param)
{
    int i, id = (int)param;
    for (i = 0; i < loop; i++)
    {
        vcSemWait(counter_sem);
        counter++;
        printf("Counter updated to %d by thread %d\n", counter, id);
        vcSemSignal(counter_sem);
    }
    return (void*)i;
}

int main()
{
    counter = 0;
    counter_sem = vcSemCreate(1);
    vcThreadQueue(SimpleThread, (void*)1);
    vcThreadQueue(SimpleThread, (void*)2);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("\nReturned:\n");
    printf("Thread 1 incremented %d times\nThread 2 incremented %d times\n", returned[0], returned[1]);
    printf("Counter: %d\n", counter);
    return 0;
}