#include "vcuserlibrary.h"
#include <time.h>

#define TEST_SIZE 10

int main()
{
    // Seed the randomizer and establish the string.
    srand(time(0));
    char str[TEST_SIZE + 1];
    str[TEST_SIZE] = '\0';
    
    // Write each character ID, then write it as a string.
    for(int i = 0; i < TEST_SIZE; i++)
    {
        str[i] = rand() % 89 + 33;
        printf("%d|", str[i]);
    }
    printf("%s\n", str);
    return 0;
}