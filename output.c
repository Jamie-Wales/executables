//
// Created by Jamie Wales on 08/11/2023.
//

#include "output.h"
#include <stdio.h>
#include "stdlib.h"
// Functions for handling errors
void recoverableError(char* message) {
    fprintf(stderr, "Error -> %s\n", message);
}

void unrecoverableError(char* message) {
    fprintf(stdout, "> %s", message);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
