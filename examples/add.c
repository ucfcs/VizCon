#include "vcuserlibrary.h"

int loop = 10;
int counter;

void* SimpleThread(void* param)
{
    int i, id = (int)param;
    for (i = 0; i < loop; i++)
    {
        counter++;
        printf("Counter updated to %d by thread %d\n", counter, id);
    }
    return (void*)i;
}

int main()
{
    counter = 0;
    vcThreadQueue(SimpleThread, (void*)1);
    vcThreadQueue(SimpleThread, (void*)2);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("\nReturned:\n");
    printf("Thread 1 incremented %d times\nThread 2 incremented %d times\n", returned[0], returned[1]);
    printf("Counter: %d\n", counter);
    return 0;
}