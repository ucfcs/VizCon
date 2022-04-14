#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 3 // Must be between 0 and 10 (exclusive).

vcMutex mtx;

// Basic thread that locks, prints the param, waits two seconds, unlocks, and leaves.
void* SimpleThread(void* param)
{
    vcMutexLock(mtx);
    printf("%d", *((int*) param));
    vcThreadSleep(1000);
    vcMutexUnlock(mtx);
    return NULL;
}

int main()
{
    if(TEST_SIZE < 1 || TEST_SIZE > 9)
    {
        printf("Invalid test size.\n");
        return 1;
    }

    mtx = vcMutexCreate();

    int params[TEST_SIZE];
    for(int i = 0; i < TEST_SIZE; i++)
    {   
        params[i] = i + 1;
        vcThreadQueue(SimpleThread, &(params[i]));
    }
    
    vcThreadStart();
    return 0;
}