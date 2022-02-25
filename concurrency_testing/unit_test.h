// VizCon libraries.
//#include "../concurrency/useroverwrite.h"
#include "../concurrency/vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 

// Cgreen testing library.
#include <cgreen/cgreen.h>

// External functions and variables that users don't need but testers may find hepful.
extern void closeAllSemaphores();
extern void closeAllMutexes();
extern CSMutex* vizconMutexListHead;
extern CSMutex* vizconMutexList;
extern CSSem* vizconSemList;
extern CSSem* vizconSemListHead;
extern CSThread* vizconThreadList;
extern CSThread* vizconThreadListHead;