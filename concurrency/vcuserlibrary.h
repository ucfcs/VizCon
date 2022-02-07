#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

#define main userMain

// Lists used to track all threads and semaphores
CSThread* vizconThreadList;
CSThread* vizconThreadListInitial;

// Thread functions
void vcThreadQueue(threadFunc func, void *arg);
void vcThreadStart();
THREAD_RET *vcThreadReturn();
