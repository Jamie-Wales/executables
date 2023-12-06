//
// Created by Jamie Wales on 27/11/2023.
//

#define BUFFSIZE 1024


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"
#include <unistd.h>
#include "output.h"
// Function to process environmental variables in the path
void process_variables(char* path) {
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == ':') {
            path[i] = ',';
        }
    }
}


// Function to tokenize the list of executables
char** tokenise(char* string, int* size) {
    const char* delims = ", "; // Delimiters for tokenizine
    char** tokenisedInput = calloc(BUFFSIZE, sizeof(char *));

    if (tokenisedInput == NULL) {
        unrecoverableError("Error -> unable to allocate memory program ending");
    }

    char* token = strtok(string, delims);
    char** head = tokenisedInput;
    *(size) = 0;

    Path* path = getPath();
    char** pathToken = 0;
    while (token != NULL) {
        int count_size = 0;
        char* buffer = (char *)malloc(BUFFSIZE);
        if (*size > BUFFSIZE) {
            free(buffer);
            unrecoverableError("Command too long");
        }

        // Handling special cases for '.' and '~'
        if (token[0] == '.') {
            char currentDir[BUFFSIZE];
            getcwd(currentDir, BUFFSIZE);
            strcpy(buffer, currentDir);
            if (strlen(token) > 1) {
                strcat(buffer, token + 1); // Append the rest of the token
            }
        }
        else if (token[0] == '~') {
            char* homeDir = path->home;
            strcpy(buffer, homeDir);
            if (strlen(token) > 1) {
                strcat(buffer, token + 1); // Append the rest of the token
            }
        }

        // If it is a path we need to retokenise
        bool retokenise = false;
        process_variables(token);
        for (int i = 0; i < strlen(token); i++) {
            if (token[i] == ',') {
                retokenise = true;
            }
        }

        // recurse retokenise
        if (retokenise) {
            pathToken = tokenise(token, &count_size);
        }
        else {
            count_size = 1;
            pathToken = &token;
        }


        // Add all elements to the array
        for (int i = 0; i < count_size; i++) {
            int tokenSize = strlen(pathToken[i]);
            *head = malloc(tokenSize + 1);
            if (*head == NULL) {
                unrecoverableError("Error -> unable to allocate memory program ending");
            }
            strncpy(*head, pathToken[i], tokenSize);
            head++;
            *size = *size + 1;
        }
        free(buffer); // Free the temporary buffer
        token = strtok(NULL, delims);
    }
    destructPath(path);
    return tokenisedInput;
}

void destructTokenInput(char** tokenisedInput, const int count) {
    char** free_buffer = tokenisedInput;
    for (int i = 0; i < count - 1; i++) {
        free(tokenisedInput[i]);
    }
    free(free_buffer);
}
