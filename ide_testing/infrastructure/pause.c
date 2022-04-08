#include "vcuserlibrary.h"

int main()
{
    // Wait 1 second.
    // This ensures the program doesn't start before console checks are complete.
    vcThreadSleep(1000);

    printf("Before");
    
    // Wait 2 seconds.
    vcThreadSleep(2000);

    printf("After");

    // Wait 1 second.
    // This ensures the program doesn't finish before button checks are complete.
    vcThreadSleep(1000);
    return 0;
}