// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "../../useroverwrite.h"
#include "../../vcuserlibrary.h"

#include <stdio.h>

vcMutex *testMutex;

THREAD_RET SimpleThread(THREAD_PARAM param)
{
    // Supress "parameter unused" compiler warning.
    int* val = param;

    // An error 504 should be thrown here.
    vcMutexLock(testMutex);
    vcMutexLock(testMutex);

    printf("I'm a thread!\n");
    
    vcMutexUnlock(testMutex);
    return 0;
}

int main(void) {
    testMutex = vcMutexCreate("Test");
    vcCobegin(SimpleThread, NULL);
    vcWaitForCompletion();
    return 0;
}