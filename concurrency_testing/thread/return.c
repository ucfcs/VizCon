// VizCon libraries.
#include "../../concurrency/vcuserlibrary.h"

// Standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// SimpleThread - A simple thread method.
//                Parameter: int val
//                Add one to the parameter and return.
//                Returns: A pointer to the parameter plus one.
void* SimpleThread(void* param)
{
    // Allocate a variable.
    int* val = (int*) malloc(sizeof(int));

    // Copy and increment the input.
    *val = *((int*) param);
    *val += 1;
    printf("Adjusted: %p, %p, %d\n", &val, val, *val);

    return (void*) val;
}

int main()
{
    // Queue a sample thread.
    int testVal = 9;
    printf("Initial:  %p, %d\n", &testVal, testVal);
    vcThreadQueue(SimpleThread, &testVal);

    // Print the address of the returned value and the value itself.
    void** returned = vcThreadReturn();
    printf("Returned: ");
    printf("%p, ", &returned[0]);
    printf("%p, ", (int*) returned[0]);
    printf("%d\n", *((int*) returned[0]));
    free(returned[0]);
    free(returned);
}
