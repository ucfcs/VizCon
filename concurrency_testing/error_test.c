// A simple program that spawns threads, mutexes and semaphores
// and then causes a fatal error
// for the purposes of checking whether the memory is properly freed.
// On POSIX, test using Valgrind: valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
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

// ErrorThread: A basic thread that causes an error.
//              Parameters: int val
//              Returns: A null pointer.
void* ErrorThread(void* param)
{
    // Supress "unused parameter" compiler warning.
    int* val = param;
    
    // Attempt to unlock an unused mutex. This will fail.
    vcMutexUnlock(mutexes[0]);

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
    vcThreadQueue(ErrorThread, &i);

    // Run the threads.
    // An error will appear once vcThreadStart starts.
    vcThreadStart();

    return 0;
}