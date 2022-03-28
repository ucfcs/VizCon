#include "vcuserlibrary.h"

// Global variable.
int counter;

// Thread.
void* SimpleThread(void* param)
{
    for (int i = 0; i < 10; i++)
        counter++;
    return NULL;
}

int real_main()
{
    // Start the threads.
    counter = 0;
    vcThreadQueue(SimpleThread, NULL);
    vcThreadQueue(SimpleThread, NULL);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("Returned: ");
    printf("%p, ", returned[0]);
    printf("Counter: %d\n", counter);
    return 0;
}