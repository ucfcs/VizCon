#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 3 // Must be at least 1.

int dummy = 0;
int global1 = 5;
char global2 = 'c';

// Basic thread that just converts the param, prints it, and then enters an endless loop.
void* SimpleThread(void* paramPtr)
{
    char paramChar = *((char*) paramPtr);
    printf("%c", paramChar);
    while(1) { dummy++; }
    return paramPtr;
}

int main()
{
    // Print the test size so the tester knows when to stop.
    if(TEST_SIZE < 1)
    {
        printf("Invalid test size.\n");
        return 1;
    }
    printf("%d|", TEST_SIZE);

    // Seed the randomizer and establish the string array.
    srand(time(0));
    char str[TEST_SIZE][4];
    int local = 7;

    // Generate a one-character string and print it.
    // Then create a thread with that name that takes the character as a parameter.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        str[i][0] = '|';
        str[i][1] = rand() % 89 + 33;
        str[i][2] = '|';
        str[i][3] = '\0';
        vcThreadQueueNamed(SimpleThread, &str[i][1], str[i]);
    }

    // Start the threads so that they'll appear in the side pane.
    vcThreadStart();
    
    return 0;
}