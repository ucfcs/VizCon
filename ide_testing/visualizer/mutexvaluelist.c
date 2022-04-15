#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 10

extern void closeAllMutexes();

int main()
{
    // Seed the randomizer and initialize the mutex.
    // Randomize the mutex status and print it.
    srand(time(0));
    vcMutex mutex = vcMutexCreate();
    if(rand() % 2 == 0)
        vcMutexLock(mutex);
    printf("%d", vcMutexStatus(mutex));

    // Each loop, randomly decide whether to change the mutex status.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        if(rand() % 2 == 0)
        {
            if(vcMutexStatus(mutex))
            {
                vcMutexLock(mutex);
                printf("%d", vcMutexStatus(mutex));
                vcThreadSleep(1000);
            }
            else
            {
                vcMutexUnlock(mutex);
                printf("%d", vcMutexStatus(mutex));
                vcThreadSleep(1000);
            }
        }

        if(rand() % 2 == 0)
        {
            vcMutexTrylock(mutex);
            printf("%d", vcMutexStatus(mutex));
            vcThreadSleep(1000);
        }
    }

    // Close the mutex.
    printf("|end");
    closeAllMutexes();
    return 0;
}