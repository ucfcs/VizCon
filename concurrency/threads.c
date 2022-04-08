#include "threads.h"

#include "lldb_lib.h"

extern int isLldbActive;


// On Windows only, define the thread runner.
#ifdef _WIN32
    DWORD winThreadRunner(LPVOID threadInput);
#endif

// createThread - Create a thread, passing in a function
//                Returns: a pointer to the thread struct.
CSThread* createThread(threadFunc func, void *arg)
{
    // Allocate the thread struct. Error out if needed.
    CSThread *thread = (CSThread*) malloc(sizeof(CSThread));
    if (thread == NULL)
        vizconError("vcThreadQueue/vcThreadQueueNamed", VC_ERROR_MEMORY);

    // Set the internal struct properties.
    thread->next = NULL;
    thread->name = NULL;
    thread->num = -1;
    thread->returnVal = NULL;

    // Save the function and argument.
    thread->func = func;
    thread->arg = arg;

    return thread;
}

// startThread - Start the thread mapped to the struct.
void startThread(CSThread* thread)
{
    if (isLldbActive)
    {
        lldb_hook_createThread(thread, thread->name);
    }
    // Platform-dependent thread start.
    #ifdef _WIN32 // Windows version
        thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)winThreadRunner, thread, CREATE_SUSPENDED, 0);
        // Attempt to resume the thread. If it fails, print an error.
        if (ResumeThread(thread->thread) == -1)
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    #elif __APPLE__ || __linux__ // POSIX version.
        // Create the thread based on the saved function and argument.
        int err;
        if (!isLldbActive)
        {
            err = pthread_create(&thread->thread, NULL, thread->func, thread->arg);
        }
        else
        {
            err = pthread_create(&thread->thread, NULL, vc_internal_thread_wrapper, thread);
        }
        if (err)
        {
            free(thread);
            vizconError("vcThreadStart/vcThreadReturn", err);
        }
    #endif

    if (isLldbActive)
    {
        lldb_waitForThreadStart();
    }
}

// winThreadRunner - A wrapper for threads on Windows that saves
//                   the function's return value whenit finishes.
//                   This is because Windows threads return a 32-bit DWORD,
//                   which is too small for some applications.
//                   Parameter: A pointer to the thread object.
//                   Returns: an unused value required by the Windows API.
#ifdef _WIN32
    DWORD winThreadRunner(LPVOID threadInput)
    {
        CSThread* thread = (CSThread*) threadInput;
        void* retval;
        if (!isLldbActive)
        {
            retval = thread->func(thread->arg);
        }
        else
        {
            retval = vc_internal_thread_wrapper(thread);
        }
        thread->returnVal = retval;
        return 0;
    }
#endif

// joinThread - Wait for the thread to finish, then join it.
void joinThread(CSThread *thread)
{
    // Platform-dependent thread joining.
    // Wait for the thread to finish.
    if (isLldbActive) {
        vcJoin(thread, NULL);
        return;
    }
    #ifdef _WIN32 // Windows version
        DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
        if(ret != WAIT_OBJECT_0)
        {
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
            return;
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
    if (isLldbActive) {
        //fprintf(stderr, "freeThread: FIXME\n");
    }
    // On Windows only, attempt to close the thread.
    // (On POSIX, the thread is closed during the join.)
    #ifdef _WIN32
        if (!CloseHandle(thread->thread))
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    #endif

    // Free the struct.
    free(thread);
}