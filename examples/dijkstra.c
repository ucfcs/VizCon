#include "vcuserlibrary.h"

int len = 5;
int BigInSmall = 20000, SmallInBig = -20000;
vcSem freeBinS, *freeSinB, *readyBinS, *readySinB;

void printArray(char* state, int* list1, int* list2)
{
    int i;
    printf("\nsmall set (%s):", state);
    for(i=0; i<len; i++)
    {
        printf(" %d", list1[i]);
    }
    printf("\nbig set   (%s):", state);
    for(i=0; i<len; i++)
    {
        printf(" %d", list2[i]);
    }
    printf("\n");
}

int GetMax(int* list, int len)
{
    int i, max = 0;
    for(i=1; i<len; i++)
    {
        if(list[i] > list[max])
        {
            max = i;
        }
    }
    return max;
}

int GetMin(int* list, int len)
{
    int i, min = 0;
    for(i=1; i<len; i++)
    {
        if(list[i] < list[min])
        {
            min = i;
        }
    }
    return min;
}

void* SmallSet(void* param)
{
    int index;
    int* list = (int*)param;
    while(BigInSmall > SmallInBig)
    {
        index = GetMax(list, len);
        vcSemWait(freeBinS);
        BigInSmall = list[index];
        vcSemSignal(readyBinS);
        vcSemWait(readySinB);
        if(BigInSmall > SmallInBig)
        {
            list[index] = SmallInBig;
        }
        vcSemSignal(freeSinB);
    }
    vcSemSignal(readyBinS);
    return (void*)1;
}

void* BigSet(void* param)
{
    int index;
    int* list = (int*)param;
    while(BigInSmall > SmallInBig)
    {
        index = GetMin(list, len);
        vcSemWait(freeSinB);
        SmallInBig = list[index];
        vcSemSignal(readySinB);
        vcSemWait(readyBinS);
        if(BigInSmall > SmallInBig)
        {
            list[index] = BigInSmall;
        }
        vcSemSignal(freeBinS);
    }
    vcSemSignal(readySinB);
    return (void*)1;
}

int real_main(void) 
{
    freeBinS = vcSemCreate(1);
    freeSinB = vcSemCreate(1);
    readyBinS = vcSemCreateInitial(0, 1);
    readySinB = vcSemCreateInitial(0, 1);
    srand(vcThreadId());
    int i;
    int array1[len], array2[len];
    for(i=0; i<len; i++)
    {
        array1[i] = rand() % 100;
        array2[i] = rand() % 100;
    }
    printArray("before", array1, array2);
    vcThreadQueue(SmallSet, array1);
    vcThreadQueue(BigSet, array2);
    vcThreadStart();
    printArray("after", array1, array2);
    return 1;
}