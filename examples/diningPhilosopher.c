#include "vcuserlibrary.h"

vcSem fork1, *fork2, *fork3, *fork4, *fork5, *room;
int m = 100;

void* Phil1(void* param)
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
    return 0;
}

void* Phil2(void* param)
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
    return 0;
}

void* Phil3(void* param)
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
    return 0;
}

void* Phil4(void* param)
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
    return 0;
}

void* Phil5(void* param)
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
    return 0;
}

int main() 
{
    fork1 = vcSemCreate(1);
    fork2 = vcSemCreate(1);
    fork3 = vcSemCreate(1);
    fork4 = vcSemCreate(1);
    fork5 = vcSemCreate(1);
    room = vcSemCreate(4);
    vcThreadQueue(Phil1, (void*)"P1");
    vcThreadQueue(Phil2, (void*)"P2");
    vcThreadQueue(Phil3, (void*)"P3");
    vcThreadQueue(Phil4, (void*)"P4");
    vcThreadQueue(Phil5, (void*)"P5");
    vcThreadStart();
    return 0;
}