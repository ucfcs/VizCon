#include "vcuserlibrary.h"

int loop = 100;

int b1, b2, b3, exitStatus;
vcSem empty1, *empty2, *empty3, *full1, *full2, *full3;

void* Producer(void* param)
{
    int r, i;
    srand(vcThreadId());
    for(i=0; i<loop; i++)
    {
        r = rand() % 100;
        vcSemWait(empty1);
        b1 = r;
        vcSemSignal(full1);
    }
    exitStatus = 1;
    return (void*)0;
}

void* CheckEven(void* param)
{
    int n;
    while(exitStatus != 1)
    {
        vcSemWait(full1);
        n = b1;
        vcSemSignal(empty1);
        vcSemWait(empty2);
        b2 = n;
        vcSemSignal(full2);
        if(n % 2 == 0)
        {
            vcSemWait(empty2);
            b2 = 0;
            vcSemSignal(full2);
        }
    }
    exitStatus = 2;
    return (void*)0;
}

void* CheckLine(void* param)
{
    int n, count = 1;
    while(exitStatus != 2)
    {
        vcSemWait(full2);
        n = b2;
        vcSemSignal(empty2);
        vcSemWait(empty3);
        b3 = n;
        vcSemSignal(full3);
        if(count % 5 == 0)
        {
            vcSemWait(empty3);
            b3 = -1;
            vcSemSignal(full3);
        }
        count = count + 1;
    }
    exitStatus = 3;
    return (void*)0;
}

void* Consumer(void* param)
{
    int k;
    while(exitStatus != 3)
    {
        vcSemWait(full3);
        k = b3;
        vcSemSignal(empty3);
        if(k < 0)
        {
            printf("\n");
        }
        else
        {
            printf("%d ", k);
        }
    }
    return (void*)0;
}

int main() 
{
    exitStatus = 0;
    empty1 = vcSemCreate(1);
    empty2 = vcSemCreate(1);
    empty3 = vcSemCreate(1);
    full1 = vcSemCreateInitial(1, 0);
    full2 = vcSemCreateInitial(1, 0);
    full3 = vcSemCreateInitial(1, 0);
    vcThreadQueue(Producer, NULL);
    vcThreadQueue(CheckEven, NULL);
    vcThreadQueue(CheckLine, NULL);
    vcThreadQueue(Consumer, NULL);
    vcThreadStart();
    return 0;
}