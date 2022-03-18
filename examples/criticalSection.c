#include "../concurrency/vcuserlibrary.h"

vcSem s;

void* P1(void* param)
{
    vcSemWait(s);
    printf("P1 is in critical section for 3 seconds\n");
    vcThreadSleep(3000);
    vcSemSignal(s);
    return 1;
}

void* P2(void* param)
{
    vcSemWait(s);
    printf("P2 is in critical section for 3 seconds\n");
    vcThreadSleep(3000);
    vcSemSignal(s);
    return 1;
}

void* P3(void* param)
{
    vcSemWait(s);
    printf("P3 is in critical section 3 seconds\n");
    vcThreadSleep(3000);
    vcSemSignal(s);
    return 1;
}

int real_main(void) 
{
    s = vcSemCreate(1);
    vcThreadQueue(P1, NULL);
    vcThreadQueue(P2, NULL);
    vcThreadQueue(P3, NULL);
    vcThreadStart();
    return 1;
}