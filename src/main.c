#include <stdio.h>

// Comment this line to disable debug messages
#define DEBUG 1

#include "include/logging.h"


int main(void) {
    loginfo("Hello, World!%i\n", 10);

    printf("Hello, World!\n");
    return 0;
}