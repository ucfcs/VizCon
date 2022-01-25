#include "threads.h"

// Create a thread, passing in a function
THREAD_RET createVCThread(threadFunc func, void *arg)
{
    VCThread *thread = (VCThread*)malloc(sizeof(VCThread));

    // Sets the next value in the linked list to NULL
    thread->next = NULL;

    void **arr = (void**)arg;

    // Create thread function for Windows
    #if defined(_WIN32) 
        thread->thread = CreateThread(NULL, 0, func, arg, 0, 0)

    // Create thread function for POSIX
    #elif defined(_APPLE_) || defined(_linux_)
    // Attempts to create a pthread. If it fails, print an error
        int err = pthread_create(&thread->thread, NULL, func, arg);
        if (err)
        {
            free(thread);
        }

        free(funct);
        return 1;
    #endif
}

void joinVCThread(VCThread *thread)
{
    // Windows join thread function
    #if defined(_WIN32)
        DWORD ret = WaitForSingleObject(thread->thread, INFINITE);

    // POSIX join thread function
    #elif defined(__APPLE__) || defined(__linux__)
    // Attempts to join the thread. If it fails, print an error
    int err = pthread_join(thread->thread, &thread->returnVal);
    if (err)
    {
        free(thread);
    }
    #endif
}

void freeVCThread(VCThread *thread)
{
    // Windows free thread function
    #if defined(_WIN32) 
        CloseHandle(thread->thread);
    #endif

    // Frees the struct
    free(thread);
}

void sleepVCThread(int milliseconds)
{
    // Windows sleep function
    #if defined(_WIN32)
        Sleep(milliseconds);
        
    // POSIX sleep function
    #elif defined(__APPLE__) || defined(__linux__)
        usleep(milliseconds * 1000);
    #endif
}