#include "vcuserlibrary.h"
#include <time.h>

#define THREAD_SIZE 3
#define TEST_SIZE 3

vcMutex mutex;

void* SimpleThread(void* param)
{
    char symbol = *((char*) param);
    for(int i = 0; i < TEST_SIZE; i++)
    {
        vcMutexLock(mutex);
        printf("%c", symbol);
        vcThreadSleep(1000);
        vcMutexUnlock(mutex);
    }
    return NULL;
}

int main()
{
    // Initialize the mutex.
    mutex = vcMutexCreate();

    // Queue and start the threads.
    char threadNames[THREAD_SIZE][4];
    for(int i = 0; i < THREAD_SIZE; i++)
    {
        threadNames[i][0] = '|';
        threadNames[i][1] = rand() % 89 + 33;
        threadNames[i][2] = '|';
        threadNames[i][3] = '\0';
        vcThreadQueueNamed(SimpleThread, &threadNames[i][1], threadNames[i]);
    }
    vcThreadStart();

    // Print the ending tag.
    printf("|end");
    return 0;
}