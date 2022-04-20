#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 5
#define TEST_STR_LEN 5

// Basic thread that just returns the parameter.
void* SimpleThread(void* param)
{
    return param;
}

int main()
{
    // Seed the randomizer and establish the string array.
    srand(time(0));
    char str[TEST_SIZE][TEST_STR_LEN + 1];

    // Generate a string and print it with a separator.
    // Then create a thread with that name.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        for(int j = 0; j < TEST_STR_LEN; j++)
            str[i][j] = rand() % 89 + 33;
        str[i][TEST_STR_LEN] = '\0';
        printf("%s", str[i]);
        if(i < TEST_SIZE - 1)
            printf("|");
        vcThreadQueueNamed(SimpleThread, NULL, str[i]);
    }

    // Start the threads so that they'll appear in the side pane.
    vcThreadStart();

    // Create two more threads but don't start them or print their names.
    // They shouldn't appear in the side pane.
    char str2[2][TEST_STR_LEN + 1];
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < TEST_STR_LEN; j++)
            str2[i][j] = rand() % 89 + 33;
        str2[i][TEST_STR_LEN] = '\0';
        vcThreadQueueNamed(SimpleThread, NULL, str2[i]);
    }
    
    return 0;
}