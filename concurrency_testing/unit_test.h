// If using MSYS to test on Windows, make sure the Windows branches are taken.
#ifdef __MSYS__
#define _WIN32
#endif

// VizCon libraries.
//#include "../concurrency/useroverwrite.h"
#include "../concurrency/vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// Cgreen testing library.
#include <cgreen/cgreen.h>

// External functions and variables that users don't need but testers may find hepful.
extern void closeAllMutexes();
extern CSMutex* vizconMutexListHead;
extern CSMutex* vizconMutexListTail;
extern CSThread* vizconThreadList;
extern CSThread* vizconThreadListHead;