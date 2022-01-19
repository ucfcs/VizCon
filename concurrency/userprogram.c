// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "useroverwrite.h"
#include "vcuserlibrary.h"

#include <stdio.h>

vcSem *fork1, *fork2, *fork3, *fork4, *fork5, *room;
int m = 20;

THREAD_RET Phil1(THREAD_PARAM param)
{
    int i;
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork1);
        vcSemWait(fork2);
        printf("%s is eating: loop %d\n", (char*)param, i);
        vcSemSignal(fork2);
        vcSemSignal(fork1);
        vcSemSignal(room);
    }
    return (THREAD_RET)1;
}

THREAD_RET Phil2(THREAD_PARAM param)
{
    int i;
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork2);
        vcSemWait(fork3);
        printf("%s is eating: loop %d\n", (char*)param, i);
        vcSemSignal(fork3);
        vcSemSignal(fork2);
        vcSemSignal(room);
    }
    return (THREAD_RET)1;
}

THREAD_RET Phil3(THREAD_PARAM param)
{
    int i;
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork3);
        vcSemWait(fork4);
        printf("%s is eating: loop %d\n", (char*)param, i);
        vcSemSignal(fork4);
        vcSemSignal(fork3);
        vcSemSignal(room);
    }
    return (THREAD_RET)1;
}

THREAD_RET Phil4(THREAD_PARAM param)
{
    int i;
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork4);
        vcSemWait(fork5);
        printf("%s is eating: loop %d\n", (char*)param, i);
        vcSemSignal(fork5);
        vcSemSignal(fork4);
        vcSemSignal(room);
    }
    return (THREAD_RET)1;
}

THREAD_RET Phil5(THREAD_PARAM param)
{
    int i;
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork5);
        vcSemWait(fork1);
        printf("%s is eating: loop %d\n", (char*)param, i);
        vcSemSignal(fork1);
        vcSemSignal(fork5);
        vcSemSignal(room);
    }
    return (THREAD_RET)1;
}

// meta: the type of this function can be up to us, weather or not it returns an int or nothing. It's name will be overwritten by useroverwrite.h
int main(void) {
    fork1 = vcSemCreate("fork1", 1);
    fork2 = vcSemCreate("fork2", 1);
    fork3 = vcSemCreate("fork3", 1);
    fork4 = vcSemCreate("fork4", 1);
    fork5 = vcSemCreate("fork5", 1);
    room = vcSemCreate("room", 4);
    vcCobegin(Phil1, (void*)"P1");
    vcCobegin(Phil2, (void*)"P2");
    vcCobegin(Phil3, (void*)"P3");
    vcCobegin(Phil4, (void*)"P4");
    vcCobegin(Phil5, (void*)"P5");
    //vcWaitForCompletion();
    int* arr = (int*) vcWaitForReturn();
    int i; 
    for(i = 0; i < 5; i++)
    {
        printf("Thread retrieved: %d\n", arr[i]);
    }
    free(arr);
    return 0;
}