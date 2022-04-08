#include "vcuserlibrary.h"

#define DEFINITION 1 // Shouldn't show up.

typedef struct testStruct
{
    int value1;
    char value2;
} TestStruct;

int main()
{
    // These should appear throughout the program.
    int numVar = 5;
    int* numPtr = &numVar;
    short tinyVar = 1;
    short* tinyPtr = &tinyVar;
    long bigVar = 1000;
    float decimalVar = 5.5;
    double bigDecVar = 100.001;
    char letterVar = 'A';
    TestStruct* testPtr = (TestStruct*) calloc(1, sizeof(TestStruct));
    TestStruct testVar = *testPtr;

    // Only the base array should appear.
    int theArray[5];
    theArray[0] = -5;

    // Everything in the loop should disappear when the loop finishes.
    for(int iterator = 0; iterator < 1; iterator++)
    {
        printf("Started loop.");
        short loopNum = iterator;
        vcThreadSleep(2000);
    }

    free(testPtr);
    return 0;
}