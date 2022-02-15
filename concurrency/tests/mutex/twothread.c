// A simple program that spaws two threads and uses a mutex.

// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
//#include "../../useroverwrite.h"
#include "../../vcuserlibrary.h"

#include <stdio.h>

vcMutex *testMutex;

THREAD_RET SimpleThread(THREAD_PARAM param)
{
    // Get the input value.
    //int val = param;

    vcMutexLock(testMutex);

    printf("I'm a thread! My value is %d\n", param);
    
    vcMutexUnlock(testMutex);
    return 0;
}

int main(void)
{
    // Create the mutex.
    testMutex = vcMutexCreate("Test");
    
    // Create two threads with different parameters.
    int nums[2] = {1, 2};
    vcThreadQueue(SimpleThread, (THREAD_PARAM)nums[0]);
    vcThreadQueue(SimpleThread, (THREAD_PARAM)nums[1]);

    // Start the threads.
    vcThreadStart();
    return 0;
}