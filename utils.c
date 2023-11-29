//
// Created by Jamie Wales on 27/11/2023.
//

#define BUFFSIZE 15


#include <stdlib.h>
#include <string.h>
#include "path.h"
#include <unistd.h>
#include "output.h"


// Function to tokenize the list of executables
char** tokenise(char* string, int* size) {
    const char* delims = ","; // Delimiters for tokenizine
    char** tokenisedInput = calloc(BUFFSIZE, sizeof(char *));

    if (tokenisedInput == NULL) {
        unrecoverableError("Error -> unable to allocate memory program ending");
    }

    char* token = strtok(string, delims);
    char** head = tokenisedInput;
    *(size) = 0;
    Path* path = getPath();

    while (token != NULL) {
        char* buffer = (char *)malloc(BUFFSIZE);
        if (*size > BUFFSIZE) {
            free(buffer);
            unrecoverableError("Command too long");
        }

        // Handling special cases for '.' and '~'
        if (token[0] == '.') {
            char* currentDir[BUFFSIZE];
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
        else {
            strcpy(buffer, token);
        }

        int tokenSize = strlen(buffer);
        *head = malloc(tokenSize + 1); // +1 for the null terminator
        if (head == NULL) {
            unrecoverableError("Error -> unable to allocate memory program ending");
        }

        strncpy(*head, buffer, tokenSize);
        (*head)[tokenSize] = '\0'; // Null terminate the string
        head++;
        *size = *size + 1;
        free(buffer); // Free the temporary buffer
        token = strtok(NULL, delims);
    }
    free(path);
    return tokenisedInput;
}

void destructTokenInput(char **tokenisedInput, int count) {
    char** free_buffer = tokenisedInput;
    for (int i = 0; i < count - 1; i++){
        free(tokenisedInput[count]);
    }
    free(free_buffer);
}
