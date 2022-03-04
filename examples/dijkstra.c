int BigInSmall = 20000, SmallInBig = -20000;
vcSem *freeBinS, *freeSinB, *readyBinS, *readySinB, *display;

char* printArray(int setInt, int stateInt, int* list, int len)
{
    int i;
    char *set, *state;
    if(setInt == 0)
    {
        set = "Small";
        vcSemWait(display);
        vcSemSignal(display);
    }
    else
    {
        set = "Big";
        vcSemWaitMult(display, 2);
    }
    if(stateInt == 0)
    {
        state = "before";
    }
    else
    {
        state = "after";
    }
    printf("%s set (%s):", set, state);
    for(i=0; i<len; i++)
    {
        printf(" %d", list[i]);
    }
    printf("\n");
    if(setInt == 1 && stateInt == 0)
    {
        printf("\n");
    }
    vcSemSignal(display);
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

THREAD_RET SmallSet(THREAD_PARAM param)
{
    int len = 5, index;
    int list[5] = {4, 1, 7, 5, 0};
    printArray(0, 0, list, len);
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
    printArray(0, 1, list, len);
    return (THREAD_RET)1;
}

THREAD_RET BigSet(THREAD_PARAM param)
{
    int len = 4, index;
    int list[4] = {1, 3, 2, 8};
    printArray(1, 0, list, len);
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
    printArray(1, 1, list, len);
    return (THREAD_RET)1;
}

int main(void) 
{
    display = vcSemCreateInitial(1, 2);
    freeBinS = vcSemCreate(1);
    freeSinB = vcSemCreate(1);
    readyBinS = vcSemCreateInitial(0, 1);
    readySinB = vcSemCreateInitial(0, 1);
    vcThreadQueue(SmallSet, NULL);
    vcThreadQueue(BigSet, NULL);
    vcThreadStart();
    return 1;
}