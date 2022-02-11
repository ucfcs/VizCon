#ifndef UTILS_H
#define UTILS_H

// Universal libraries
#include <stdio.h>
#include <stdlib.h>

// Windows libraries and definitions
#ifdef _WIN32 
    #include <windows.h>

// Linux and MacOS's libraries and definitions
#elif __linux__ || __APPLE__ 
    #include <pthread.h>
    #include <errno.h>
#endif

//Other functions
char* vizconCreateName(int type, int value);
int vizconStringLength(char* name);
void vizconError(char* func, int err);

#endif