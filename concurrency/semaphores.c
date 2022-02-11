#include "semaphores.h"

//Create a CSSem
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue)
{
    if(name == NULL)
    {
        return NULL;
    }
    CSSem* sem = (CSSem*)malloc(sizeof(CSSem));
    if (sem == NULL) 
    {
        vizconError("vcSemCreate", 502);
    }
    sem->next = NULL;
    sem->name = (char*)name;
    sem->num = -1;
    #if defined(_WIN32) // windows
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    if(sem->sem == NULL)
    {
        int err = (int)GetLastError();
        free(sem);
        vizconError("vcSemCreate", err);
    }
    sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    if(sem->sem == SEM_FAILED)
    {
        free(sem);
        vizconError("vcSemCreate", errno);
    }
    if(sem_unlink(name))
    {
        free(sem);
        vizconError("vcSemCreate", errno);
    }
    sem->count = maxValue;
    #endif
    return sem;
}

//Releases 1 permit from semaphore and increment its count
void semSignal(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!ReleaseSemaphore(sem->sem, 1, NULL))
    {
        vizconError("vcSemSignal", GetLastError());
    }
    sem->count = sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_post(sem->sem))
    {
        vizconError("vcSemSignal", errno);
    }
    sem->count = sem->count + 1;
    #endif
}

//Waits for semaphore to become available, attaining 1 permit from semaphore and decrementing its count
void semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError("vcSemWait", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcSemWait", 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError("vcSemWait", 501);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_wait(sem->sem))
    {
        vizconError("vcSemWait", errno);
    }
    #endif
}

//Try to attain 1 permit from semaphore and decrement its count
//Returns immediately if semaphore has no remaining permits at time of call
//returns 1 if available, else 0
int semTryWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, 0);
    if(ret == WAIT_FAILED)
    {
        vizconError("vcSemTryWait", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcSemTryWait", 500);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(!sem_trywait(sem->sem))
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(errno != EAGAIN)
    {
        vizconError("vcSemTryWait", errno);
    }
    #endif
    return 0;
}

//Returns semaphore's current value
int semValue(CSSem* sem)
{
    return sem->count;
}

//Frees all data associated with a CSSem type, including itself
void semClose(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!CloseHandle(sem->sem))
    {
        vizconError("vcSemClose", GetLastError());
    }
    free(sem);
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_close(sem->sem))
    {
        vizconError("vcSemClose", errno);
    }
    free(sem);
    #endif
}