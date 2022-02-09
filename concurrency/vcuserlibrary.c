// Create a thread instance with arguments
void vcThreadQueue(threadFunc func, void *arg)
{
    // Attempts to create the thread
    CSThread *thread = createThread(func, arg);

    // If there are no threads in the list, make the current thread the initial one
    if (vizconThreadListInitial == NULL)
    {
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
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
}

// Start all threads created by vcThreadQueue and return their results
THREAD_RET *vcThreadReturn()
{
    int i = 0;

    if (vizconThreadListinitial = NULL)
    {
        return;
    }

    // Begin all threads and get the number of threads
    vizconThreadList = vizconThreadlistInitial;
    
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

    return arr;
}