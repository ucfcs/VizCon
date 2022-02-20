#ifndef UTILS_H
#define UTILS_H

// Universal libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows libraries and definitions
#ifdef _WIN32 
    #include <windows.h>

// POSIX libraries and definitions
#elif __linux__ || __APPLE__ 
    #include <pthread.h>
    #include <errno.h>
#endif

// VizCon type codes used by vizconCreateName
#define VC_TYPE_THREAD 0
#define VC_TYPE_SEM 1
#define VC_TYPE_MUTEX 2

// VizCon error codes
#define VC_ERROR_ABANDONED 500
#define VC_ERROR_TIMEOUT 501
#define VC_ERROR_MEMORY 502
#define VC_ERROR_BADCOUNT 503
#define VC_ERROR_NAMEERROR 504
#define VC_ERROR_DOUBLEUNLOCK 510
#define VC_ERROR_DOUBLELOCK 511
#define VC_ERROR_CROSSTHREADUNLOCK 512

// Function prototypes
char* vizconCreateName(int type, int value);
int vizconStringLength(char* name);
void vizconError(char* func, int err);

#endif