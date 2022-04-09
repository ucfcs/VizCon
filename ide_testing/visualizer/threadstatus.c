#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 3 // Must be between 0 and 10 (exclusive).

vcSem mtx;

// Basic thread that locks, prints the param, waits two seconds, unlocks, and leaves.
void* SimpleThread(void* param)
{
    vcSemWait(mtx);
    printf("%d", *((int*) param));
    vcThreadSleep(2500);
    vcSemSignal(mtx);
    return NULL;
}

int main()
{
    if(TEST_SIZE < 1 || TEST_SIZE > 9)
    {
        printf("Invalid test size.\n");
        return 1;
    }

    mtx = vcSemCreate(1);

    int params[TEST_SIZE];
    for(int i = 0; i < TEST_SIZE; i++)
    {   
        params[i] = i + 1;
        vcThreadQueue(SimpleThread, &(params[i]));
    }
    
    vcThreadStart();
    return 0;
}