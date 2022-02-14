#include "threads.h"

// Create a thread, passing in a function
CSThread* createThread(threadFunc func, void *arg)
{
	// Attempts to create the Thread data type. If it fails, print an error
    CSThread *thread = (CSThread*)malloc(sizeof(CSThread));
	if (thread == NULL)
		vizconError("vcThreadQueue", 8);

    // Sets the next value in the linked list to NULL
    thread->next = NULL;
    thread->name = NULL;
    thread->num = -1;

    // Create thread function for Windows
    #if defined(_WIN32) 
        thread->thread = CreateThread(NULL, 0, func, arg, CREATE_SUSPENDED, 0);

    // Create thread function for POSIX
    #elif defined(_APPLE_) || defined(_linux_)
		// Passes in the parameters as arguments for future creation
		thread->func = func;
		thread->arg = arg;
    #endif
	
	return thread;
}

void joinThread(CSThread *thread)
{
    // Windows join thread function
    #if defined(_WIN32)
		// Attempt to wait for the object, rpnting the appropriate error if it fails
        DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
		if (ret == WAIT_FAILED)
		{
			vizconError("vcThreadStart/vcThreadReturn", GetLastError());
		}
		else if (ret == WAIT_OBJECT_0)
		{
			if (!GetExitCodeThread(thread->thread, &thread->returnVal))
			{
				vizconError("vcThreadStart/vcThreadReturn", GetLastError());
			}
		}
		else if(ret == WAIT_ABANDONED)
		{
			vizconError("vcThreadStart/vcThreadReturn", 500);
		}
		else if (ret == WAIT_TIMEOUT)
		{
			vizconError("vcThreadStart/vcThreadReturn", 501);
		}

    // POSIX join thread function
    #elif defined(__APPLE__) || defined(__linux__)
    // Attempts to join the thread. If it fails, print an error
    int err = pthread_join(thread->thread, &thread->returnVal);
    if (err)
    {
        free(thread);
		vizconError("vcThreadStart/vcThreadReturn", err);
    }
    #endif
}

void freeThread(CSThread *thread)
{
    // Windows free thread function
    #if defined(_WIN32) 
		// Attempts to close the thread. If it fails, print an error
        if (!CloseHandle(thread->thread))
		{
			vizconError("vcThreadStart/vcThreadReturn", GetLastError());
		}
    #endif

    // Frees the struct
    free(thread);
}

//Start a given thread that was in a suspended state
void startThread(CSThread* thread)
{
    #if defined(_WIN32) // windows
	// Attempts to resume a thread. If it fails, print an error
    if (ResumeThread(thread->thread) == -1)
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    #elif defined(__APPLE__) || defined(__linux__)
	// Attempts to create a pthread using the previously passed in parameters. If it fails, print an error
    int err = pthread_create(&thread->thread, NULL, thread->func, thread->arg);
    if (err)
    {
        free(thread);
        vizconError("vcThreadQueue", err);
    }
    #endif
}