#include "vcuserlibrary.h"

int loop = 10;
int counter;
vcMutex counter_mutex;

void* SimpleThread(void* param)
{
    int i, id = (int)param;
    for (i = 0; i < loop; i++)
    {
        vcMutexLock(counter_mutex);
        counter++;
        printf("Counter updated to %d\n", counter);
        vcMutexUnlock(counter_mutex);
    }
    return (void*)i;
}

int main()
{
    counter = 0;
    counter_mutex = vcMutexCreate();
    vcThreadQueue(SimpleThread, NULL);
    vcThreadQueue(SimpleThread, NULL);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("\nReturned:\n");
    printf("Thread 1 incremented %d times\nThread 2 incremented %d times\n", returned[0], returned[1]);
    printf("Counter: %d\n", counter);

    return 0;
}