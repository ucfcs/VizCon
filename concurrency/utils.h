#ifndef UTILS_H
#define UTILS_H

//universal libraries
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32 // window's libraries and definitions
    #include <windows.h>
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
    #include <pthread.h>
    #include <errno.h>
#endif

//Other functions
char* vizconCreateName(int type, int value);
int vizconStringLength(char* name);
void vizconError(char* func, int err);

#endif