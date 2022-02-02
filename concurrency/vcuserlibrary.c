#include "vcuserlibrary.h"

VCMutex* mutexListHead;
VCMutex* mutexListTail;

VCMutex* vcMutexCreate(char* name)
{
    // Make a new VCMutex struct.
    VCMutex* newVCMutex;

    // Make a new CSMutex and set it as newVCMutex's contents.
    CSMutex* newMutex = mutexCreate(name);
    newVCMutex->mutex = newMutex;

    // Add newVCMutex to the end of the list and adjust mutexListTail.
    newVCMutex->next = NULL;
    if(mutexListHead == NULL)
    {
        mutexListHead = newVCMutex;
        mutexListTail = newVCMutex;
    }
    else
    {
        mutexListTail->next = newVCMutex;
        mutexListTail = newVCMutex;
    }
    
    return newVCMutex;
}

void vcMutexLock(VCMutex* mutex)
{
    mutexLock(mutex->mutex);
}

int vcMutexTrylock(VCMutex* mutex)
{
    mutexTryLock(mutex->mutex);
}

void vcMutexUnlock(VCMutex* mutex)
{
    mutexUnlock(mutex->mutex);
}

int vcMutexStatus(VCMutex* mutex)
{
    mutexStatus(mutex->mutex);
}