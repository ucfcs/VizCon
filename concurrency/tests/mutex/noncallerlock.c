// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "../../useroverwrite.h"
#include "../../vcuserlibrary.h"

#include <stdio.h>

vcMutex *testMutex;

THREAD_RET ParentThread(THREAD_PARAM param)
{
    // Supress "parameter unused" compiler warning.
    int* val = param;

    vcMutexLock(testMutex);
    
    printf("I'm a parent thread.\n");

    // Give the child five seconds to interfere.
    #ifdef _WIN32
    Sleep(5000);
    #elif __linux__
    sleep(5);
    #endif

    vcMutexUnlock(testMutex);
    return 0;
}

THREAD_RET ChildThread(THREAD_PARAM param)
{
    // Supress "parameter unused" compiler warning.
    int* val = param;

    // Give the parent three seconds to start up.
    #ifdef _WIN32
    Sleep(3000);
    #elif __linux__
    sleep(3);
    #endif

    printf("I'm a child thread.\n");
    
    // An error 503 should be thrown here.
    vcMutexUnlock(testMutex);

    printf("Got the lock!\n");
    return 0;
}

int main(void) {
    testMutex = vcMutexCreate("Test");
    int vals[2];
    vals[0] = 1;
    vals[1] = 2;
    vcCobegin(ParentThread, NULL);
    vcCobegin(ChildThread, NULL);
    vcWaitForCompletion();
    return 0;
}