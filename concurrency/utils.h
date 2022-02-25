// If using MSYS to test on Windows, make sure the Windows branches are taken.
#ifdef __MSYS__
#define _WIN32
#endif

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

// VizCon error codes.
// 50X - Error with object creation
#define VC_ERROR_MEMORY 500
#define VC_ERROR_NAMEERROR 501
#define VC_ERROR_BADCOUNT 502

// 51X - Error with mutex permissions
#define VC_ERROR_DOUBLEUNLOCK 510
#define VC_ERROR_DOUBLELOCK 511
#define VC_ERROR_CROSSTHREADUNLOCK 512

// 52X - Unexpected system response (Windows only)
#define VC_ERROR_ABANDONED 520
#define VC_ERROR_TIMEOUT 521

// Other constants
#define MAX_ERROR_MESSAGE_LENGTH 200

// Function prototypes
char* vizconCreateName(int type, int value);
int vizconStringLength(char* name);
void vizconError(char* func, int err);