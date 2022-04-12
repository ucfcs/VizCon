#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 3

extern void closeAllSemaphores();

int main()
{
    // Seed the randomizer, initialize the semaphore, and print its max value.
    srand(time(0));
    vcSem sem = vcSemCreate((rand() % 24) + 1);
    printf("%d", vcSemValue(sem));

    // Each loop, remove a random amount of permits and pause,
    // then add a random number back and pause again.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        int removed = rand() % vcSemValue(sem);
        vcSemWaitMult(sem, removed);
        printf("|%d", vcSemValue(sem));
        vcThreadSleep(1000);

        // If returned is 0, the mod function would otherwise fail.
        int returned;
        if(removed == 0)
            returned = 0;
        else
            returned = rand() % removed;
        vcSemSignalMult(sem, returned);
        printf("|%d", vcSemValue(sem));
        vcThreadSleep(1000);

        // If the sem value is zero, getting "removed" will fail on the next loop.
        if(vcSemValue(sem) == 0)
            vcSemSignal(sem);
    }

    // Close the semaphore.
    closeAllSemaphores();
    printf("|new");
    vcThreadSleep(1000);

    // Create another which has a different start and max.
    // Then print the max (the tester will calculate the expected value.)
    int max = (rand() % 24) + 1;
    sem = vcSemCreateInitial(max, max / 2);
    printf("|%d", max);
    vcThreadSleep(1000);

    // Close the semaphore again.
    closeAllSemaphores();
    return 0;
}