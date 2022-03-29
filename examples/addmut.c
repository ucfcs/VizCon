#include "vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

int counter;
CSMutex *counter_mutex;
void* SimpleThread(void* param)
{
    for (int i = 0; i < 10; i++)
    {
        vcMutexLock(counter_mutex);
        counter++;
        printf("Counter updated to %d\n", counter);
        vcMutexUnlock(counter_mutex);
    }
    return NULL;
}

int real_main()
{
    printf("Hello world!\n");
    counter = 0;
    counter_mutex = vcMutexCreate();
    vcThreadQueue(SimpleThread, NULL);
    vcThreadQueue(SimpleThread, NULL);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("Returned: ");
    printf("%p, ", returned[0]);
    printf("Counter: %d\n", counter);

    return 0;
}
