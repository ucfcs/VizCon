#include "concurrencylib.h"

int main(int argc, char** argv) {
    //Begin the user program
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n\n");
    userMain();
    printf("\nBack in %s\n", __func__);
    return 0;
}