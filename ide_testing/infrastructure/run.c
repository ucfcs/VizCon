#include "vcuserlibrary.h"

int main()
{
    // Wait 1 second.
    // This ensures the program doesn't finish before button checks are complete.
    vcThreadSleep(1000);

    printf("Hello, World!");
    return 0;
}