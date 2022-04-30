// A simple program that spawns two threads and then halts in the middle of them
// for the purposes of checking whether the memory is properly freed.
// On Linux, test using Valgrind: valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
// On macOS, test using Leaks: leaks -atExit -- ./a.out
// On Windows, test using Dr. Memory: drmemory -- a.exe

//#include "../../concurrency/useroverwrite.h"
#include "../../concurrency/vcuserlibrary.h"
#include <unistd.h>

// ThreadOne: Wait five seconds and return.
//            Parameters: int val
//            Returns: A null pointer.
void* ThreadOne(void* param)
{
    // Allocate a variable, wait five seconds, and return.
    int* val = (int*) param;
    
    // Platform-dependent 50 second (50000 millisecond) wait.
    #ifdef _WIN32 // Windows version
        Sleep(50000);
    #elif __linux__ || __APPLE__ // POSIX version
        sleep(50);
    #endif

    return NULL;
}

// ThreadTwo: A basic thread that halts after two seconds.
//            Parameters: int val
//            Returns: A null pointer.
void* ThreadTwo(void* param)
{
    // Supress "unused parameter" compiler warning.
    int* val = param;

    // Platform-dependent 2 second (2000 millisecond) wait.
    #ifdef _WIN32 // Windows version
        Sleep(2000);
    #elif __linux__ || __APPLE__ // POSIX version
        sleep(2);
    #endif
    
    // Halt.
    vcHalt(0);

    return NULL;
}

int main(void)
{
    // Create the objects.
    int i = 0;
    vcThreadQueue(ThreadOne, &i);
    vcThreadQueue(ThreadTwo, &i);

    // Run the threads.
    // An error will appear once ErrorThread starts.
    vcThreadStart();

    return 0;
}
