#include "vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

int counter;
void* SimpleThread(void* param)
{
    for (int i = 0; i < 10; i++)
    {
        counter++;
    }
    return NULL;
}

int real_main()
{
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