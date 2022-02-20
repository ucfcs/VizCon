#include "threads.h"

// createThread - Create a thread, passing in a function
//                Returns: a pointer to the thread struct.
CSThread* createThread(threadFunc func, void *arg)
{
    // Allocate the thread struct. Error out if needed.
    CSThread *thread = (CSThread*) malloc(sizeof(CSThread));
    if (thread == NULL)
        vizconError("vcThreadQueue/vcThreadQueueNamed", VC_ERROR_MEMORY);

    // Set the non-platform-dependent properties of the struct.
    thread->next = NULL;
    thread->name = NULL;
    thread->num = -1;
    thread->returnVal = NULL;

    // Platform-dependent thread definition.
    #if defined(_WIN32) // Windows version
        // Create the thread in a suspended state.
        // It will be woken up by a vcThreadStart or vcThreadReturn.
        thread->thread = CreateThread(NULL, 0, func, arg, CREATE_SUSPENDED, 0);

    #elif defined(__APPLE__) || defined(__linux__) // POSIX version
        // POSIX cannot create suspended threads, so save everything for later.
        // The thread will be created by a vcThreadStart or vcThreadReturn.
        thread->func = func;
        thread->arg = arg;

    #endif
    
    return thread;
}

// joinThread - Wait for the thread to finish, then join it.
void joinThread(CSThread *thread)
{
    // Platform-dependent thread joining.
    // Attempt to wait for the object.
    #ifdef _WIN32 // Windows version
        DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
        switch(ret)
        {          
            // WAIT_OBJECT_0 - The thread ended successfully.
            //                 Save the return value.
            case WAIT_OBJECT_0:
            {
                if (!GetExitCodeThread(thread->thread, &thread->returnVal))
                    vizconError("vcThreadStart/vcThreadReturn", GetLastError());
                return;
            }

            // Default - OS-level error.
            default:
            {
                vizconError("vcThreadStart/vcThreadReturn", GetLastError());
                return;
            }
        }

    #elif __APPLE__ || __linux__ // POSIX version
    int err = pthread_join(thread->thread, &thread->returnVal);
    if (err)
    {
        free(thread);
        vizconError("vcThreadStart/vcThreadReturn", err);
    }

    #endif
}

// freeThread - Close the thread (if needed) and free the struct.
void freeThread(CSThread *thread)
{
    // On Windows only, attempt to close the thread.
    // (On POSIX, the thread is closed during the join.)
    #ifdef _WIN32
        if (!CloseHandle(thread->thread))
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    #endif

    // Free the struct.
    free(thread);
}

// startThread - Start the thread mapped to the struct.
void startThread(CSThread* thread)
{
    // Platform-dependent thread start.
    #ifdef _WIN32 // Windows version
    // Attempt to resume the thread. If it fails, print an error.
    if (ResumeThread(thread->thread) == -1)
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    
    #elif __APPLE__ || __linux__
    // Create the thread based on the saved function and argument.
    int err = pthread_create(&thread->thread, NULL, thread->func, thread->arg);
    if (err)
    {
        free(thread);
        vizconError("vcThreadStart/vcThreadReturn", err);
    }
    
    #endif
}