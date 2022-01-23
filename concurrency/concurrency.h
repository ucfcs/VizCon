#ifndef CONCURRENCYLIB_H
#define CONCURRENCYLIB_H
//universal libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32 // window's libraries and definitions
#include <windows.h>
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
#include <errno.h>
#include <unistd.h>
#endif

// meta: proto type for the compiled name of the user function so we have access to it in our program
int userMain(void); 

//Functions
void vizconError(int func, int err);

#endif