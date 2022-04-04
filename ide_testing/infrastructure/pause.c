#include "vcuserlibrary.h"

int main()
{
    printf("Before");
    
    // Wait 2 seconds.
    #ifdef _WIN32
    Sleep(2000);
    #else
    sleep(2);
    #endif

    printf("After");
    return 0;
}