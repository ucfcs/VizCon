// VizCon libraries.
#include "vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
//#include <windows.h>

int counter;
CSSem *counter_sem;
void* SimpleThread(void* param)
{
    for (int i = 0; i < 10; i++)
    {
        vcSemWait(counter_sem);
        counter++;
        vcSemSignal(counter_sem);
    }
    return NULL;
}
extern int isLldbActive;
int real_main()
{
    counter = 0;
    counter_sem = vcSemCreate(1);
    vcThreadQueue(SimpleThread, NULL);
    vcThreadQueue(SimpleThread, NULL);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("Returned: ");
    printf("%p, ", returned[0]);
    printf("Counter: %d\n", counter);
    //if (isLldbActive)
    //    Sleep(5000);
    return 0;
}