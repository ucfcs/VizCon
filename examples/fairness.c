int b, n;

void* P1(void* param)
{
    while(b)
    {
        n++;
    }
    return 1;
}

void* P2(void* param)
{
    b = 0;
    return 1;
}

int real_main(void) 
{
    int i, trials = 10;
    for(i=0; i<trials; i++)
    {
        n = 0;
        b = 1;
        vcThreadQueue(P1, NULL);
        vcThreadQueue(P2, NULL);
        vcThreadStart();
        printf("Trial %d has P1 loop %d times before P2's first action\n", i+1, n);
    }
    return 1;
}