#include "vcuserlibrary.h"

// Create a thread instance with arguments
void vcThreadQueue(threadFunc func, void *arg)
{
    // Attempts to create the thread
    CSThread *thread = createThread(func, arg);

    // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListInitial == NULL)
    {
        thread->name = vizconCreateName(0, 0);
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
        thread->name = vizconCreateName(0, vizconThreadList->num + 1);
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

/* Create a thread instance with arguments, takes in a third
parameter allowing the user to assign a name to the thread */
void vcThreadQueueNamed(threadFunc func, void *arg, char *name)
{
    CSThread *thread = createThread(func, arg);

    // Attempts to allocate space for the thread name
    char *mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName = NULL)
    {
        vizconError("vcThreadQueueNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    thread->name = mallocName;

        // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListInitial == NULL)
    {
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
}

// Start all threads created by vcThreadQueue
void vcThreadStart()
{
    // If there are no threads in the list, return immediately
    if (vizconThreadListInitial = NULL)
    {
        return;
    }

    // Begin all threads
    vizconThreadList = vizconThreadListInitial;

    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    // Waits for all threads to complete
    vizconThreadList = vizconThreadListInitial;
    
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    freeThreads();
}

// Start all threads created by vcThreadQueue and return their results
THREAD_RET *vcThreadReturn()
{
    int i = 0;

    if (vizconThreadListInitial = NULL)
    {
        return;
    }

    // Begin all threads and get the number of threads
    vizconThreadList = vizconThreadListInitial;
    
    while (vizconThreadList != NULL)
    {
        startThread(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    THREAD_RET *arr = (THREAD_RET*)malloc(sizeof(THREAD_RET) * i);

    // Wait for all threads to complete and get return values
    vizconThreadList = vizconThreadListInitial;

    i = 0;
    while (vizconThreadList != NULL)
    {
        joinThread(vizconThreadList);
        arr[i] = vizconThreadList->returnVal;
        vizconThreadList = vizconThreadList->next;
        i++;
    }

    freeUserThreads();

    return arr;
}

// Frees all user threads
void freeUserThreads()
{
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        free(vizconThreadListInitial->name);
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }
}