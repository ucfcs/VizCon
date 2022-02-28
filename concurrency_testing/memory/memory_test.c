// A simple program that spawns threads, mutexes and semaphores
// for the purposes of checking whether the memory is properly freed.
// On Linux, test using Valgrind: valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
// On macOS, test using Leaks: leaks -atExit -- ./a.out
// On Windows, test using Dr. Memory: drmemory -- a.exe

//#include "../concurrency/useroverwrite.h"
#include "../concurrency/vcuserlibrary.h"

#define SPAWN_COUNT 100

vcSem semaphores[SPAWN_COUNT];
vcMutex mutexes[SPAWN_COUNT];

// SimpleThread: A basic thread that exists.
//               Parameters: int val
//               Returns: A null pointer.
void* SimpleThread(void* param)
{
    // Allocate a variable, set it to param + 1, and return.
    int* val = (int*) param;
    *val += 1;
    return NULL;
}

// ReturnThread: A basic thread that returns something.
//               Parameters: int val
//               Returns: A pointer to val + 1.
void* ReturnThread(void* param)
{
    // Allocate a variable, set it to param + 1, and return.
    int* val = (int*) calloc(1, sizeof(int));
    *val = *((int*) param) + 1;
    return (void*) val;
}

int main(void)
{
    // Create the objects.
    int i;
    for(i = 0; i < SPAWN_COUNT; i++)
    {
        vcThreadQueue(SimpleThread, &i);
        semaphores[i] = vcSemCreate(2);
        mutexes[i] = vcMutexCreate();
    }

    // Run the threads.
    // This should free every object except the return values.
    vcThreadStart();

    // Repeat the process, this time with vcThreadReturn.
    for(i = 0; i < SPAWN_COUNT; i++)
    {
        vcThreadQueue(SimpleThread, &i);
        semaphores[i] = vcSemCreate(2);
        mutexes[i] = vcMutexCreate();
    }

    // Run the threads and get the return values.
    // This should free every object except the return values.
    void** returned = vcThreadReturn();

    // Free the return values.
    for(i = 0; i < SPAWN_COUNT; i++)
        free(returned[i]);
    free(returned);

    return 0;
}