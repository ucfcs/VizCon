// A file meant to test feature parity with jBACI using an assignment written in C--

// VizCon libraries
#include "../concurrency/vcuserlibrary.h"

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// Semaphores
vcSem full;
vcSem empty;
vcMutex mutex;

// Constants
#define SIZE 5 // Maximum size of buffer
#define MAX 20 // Maximum number of items to produce/consume

// Global variables
int in = 0;
int out = 0;
int buffer[SIZE];

threadFunc producer(int pNum)
{
    int item = 0; // The produced item
    int count = 0;

    while (count < MAX)
    {
        vcSemWait(empty); // Wait when there are no empty slots
        vcMutexLock(mutex); // Obtain exclusive access to standard output

        // Critical section

        item += 1;
        printf("Producer %d produced item %d\n", pNum, item);
        buffer[in] = item; // Store the item
        in = (in + 1) % SIZE; // Determine next location in buffer to put next item
        count += 1;

        // End of critical section

        vcMutexUnlock(mutex);
        vcSemSignal(full); // Signal to consumer that buffer has some data and they can consume now
    }

    vcSemWait(empty);
    vcMutexLock(mutex); 
    printf("Producer %d produced item %d\n", pNum, item);
    vcMutexUnlock(mutex);
    vcSemSignal(full); 
}

threadFunc consumer(int cNum)
{
    int item = 0;
    int count = 0;

    while (count < MAX)
    {
        vcSemWait(full); // Wait/sleep when there are no full slots
        vcMutexLock(mutex);

        // Critical section

        item = buffer[out];

        if (item > 0)
        {
            count += 1;
            printf("Consumer %d consumed item %d from the buffer\n", cNum, count);
            out = (out + 1) % SIZE; // Determine next location in the buffer to consume and item
        }

        // End of critical section

        vcMutexUnlock(mutex);
        vcSemSignal(empty); // Signal the producer that buffer slots are emptied and they can produce more
    }

    vcSemWait(full);
    vcMutexLock(mutex);
    printf("Consumer %d consumed %d items\n", cNum, count);
    vcMutexUnlock(mutex);
    vcSemSignal(empty);
}

int main()
{
    int pOne = 1;
    int cOne = 1;
    int pTwo = 2;
    int cTwo = 2;
    int pThree = 3;
    int cThree = 3;

    
    // One producer/consumer pair
    full = vcSemCreate(SIZE);
    empty = vcSemCreateInitial(0, SIZE);
    mutex = vcMutexCreate();

    // Queue threads
    vcThreadQueue(producer, &pOne);
    vcThreadQueue(consumer, &cOne);
    vcThreadQueue(producer, &pTwo);
    vcThreadQueue(consumer, &cTwo);
    vcThreadQueue(producer, &pThree);
    vcThreadQueue(consumer, &cThree);

    // Start threads
    vcThreadStart();
    
}