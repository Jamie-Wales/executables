//
// Created by Jamie Wales on 08/11/2023.
//

#include "output.h"
#include <stdio.h>
#include "stdlib.h"
// Functions for handling errors
void recoverableError(char* errorMessage) {
    fprintf(stderr, "Error -> %s\n", errorMessage);
}

void unrecoverableError(char* errorMessage) {
    fprintf(stdout, "> %s", errorMessage);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
