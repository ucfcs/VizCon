vcSem s;

THREAD_RET P1(THREAD_PARAM param)
{
    vcSemWait(s);
    printf("P1 is in critical section\n");
    vcThreadSleep(1);
    vcSemSignal(s);
    return 1;
}

THREAD_RET P2(THREAD_PARAM param)
{
    vcSemWait(s);
    printf("P2 is in critical section\n");
    vcThreadSleep(1);
    vcSemSignal(s);
    return 1;
}

THREAD_RET P3(THREAD_PARAM param)
{
    vcSemWait(s);
    printf("P3 is in critical section\n");
    vcThreadSleep(1);
    vcSemSignal(s);
    return 1;
}

int main(void) 
{
    s = vcSemCreate(1);
    vcThreadQueue(P1, NULL);
    vcThreadQueue(P2, NULL);
    vcThreadQueue(P3, NULL);
    vcThreadStart();
    return 1;
}