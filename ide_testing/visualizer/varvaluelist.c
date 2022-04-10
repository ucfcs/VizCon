#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 3

// Basic thread that just returns the parameter.
void* SimpleThread(void* param)
{
    return param;
}

int main()
{
    // Seed the randomizer and initialize the variable.
    srand(time(0));
    int testInt = rand();
    double testDouble = rand() + ((rand() % 100) / 100.0);
    char testChar = rand() % 89 + 35;

    // Allocate a large number of pointers.
    void* pointerVals[TEST_SIZE];
    for(int i = 0; i < TEST_SIZE; i++)
        pointerVals[i] = malloc(sizeof(int));
    void* testPtr = pointerVals[0];
    printf("%d~%.2lf~%c~0x%p\n", testInt, testDouble, testChar, testPtr);

    // Change each variable, then print the new values.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        vcThreadSleep(1000);
        testInt = rand();
        testDouble = rand() + ((rand() % 100) / 100.0);
        testChar = rand() % 89 + 35;
        testPtr = pointerVals[rand() % TEST_SIZE];
        printf("|%d~%.2lf~%c~0x%p\n", testInt, testDouble, testChar, testPtr);
    }

    // Free the allocated pointers.
    for(int i = 0; i < TEST_SIZE; i++)
        free(pointerVals[i]);
    printf("|Done.");
    return 0;
}