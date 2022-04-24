// If using MSYS to test on Windows, make sure the Windows branches are taken.
#ifdef __MSYS__
    #define _WIN32
#endif

// Universal libraries
#include <stdio.h>
#include <stdlib.h>

// Platform-dependent libraries
#ifdef _WIN32 
    #include <windows.h>
#elif __linux__ || __APPLE__ 
    #include <pthread.h>
    #include <errno.h>
    #include <sys/syscall.h>
    #define gettid() syscall(SYS_gettid)
#endif

// VizCon error codes
// X - Error with object creation
#define VC_ERROR_MEMORY -1
#define VC_ERROR_BADCOUNT -2

// 1X - Error with permissions
#define VC_ERROR_DOUBLEUNLOCK -11
#define VC_ERROR_DOUBLELOCK -12
#define VC_ERROR_CROSSTHREADUNLOCK -13
#define VC_ERROR_SEMVALUELIMIT -14
#define VC_ERROR_CREATEDISABLED -15
#define VC_ERROR_THREADSACTIVE -16

// 2X - Unexpected system response (Windows only)
#define VC_ERROR_ABANDONED -21
#define VC_ERROR_TIMEOUT -22