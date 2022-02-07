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