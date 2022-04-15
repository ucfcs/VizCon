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
#endif

// VizCon error codes
// 50X - Error with object creation
#define VC_ERROR_MEMORY 500
#define VC_ERROR_NAMEERROR 501
#define VC_ERROR_BADCOUNT 502

// 51X - Error with permissions
#define VC_ERROR_DOUBLEUNLOCK 510
#define VC_ERROR_DOUBLELOCK 511
#define VC_ERROR_CROSSTHREADUNLOCK 512
#define VC_ERROR_SEMVALUELIMIT 513
#define VC_ERROR_CREATEDISABLED 514
#define VC_ERROR_THREADSACTIVE 515

// 52X - Unexpected system response (Windows only)
#define VC_ERROR_ABANDONED 520
#define VC_ERROR_TIMEOUT 521

// Other constants
#define MAX_ERROR_MESSAGE_LENGTH 200

// Function prototypes
void vizconError(char* func, int err);       // Reports an error and then closes the program