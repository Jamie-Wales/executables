//
// Created by Jamie Wales on 27/11/2023.
//

#define BUFFSIZE 1024


#include <stdlib.h>
#include <string.h>
#include "path.h"
#include <unistd.h>
#include "output.h"
// Function to process environmental variables in the path
char* process_variables(char* path) {
    if (path == NULL) {
        return NULL;
    }
    // Check if path begins with '$', indicating an environment variable
    if (path[0] == '$') {
        char* variable = getenv(path + 1);  // Retrieve the environment variable
        if (variable == NULL) {
            return NULL;  // Environment variable not found
        }
        char* buffer = malloc(strlen(variable) + 1);  // Allocate memory for the variable
        if (buffer == NULL) {
            return NULL;  // Memory allocation failed
        }

        strcpy(buffer, variable);
        for (int i = 0; i < strlen(variable) - 1; i++) {
            if (buffer[i] == ':') {
                buffer[i] = ',';
            }
        }
        return buffer;

    }
    return path; // Return the original path if no environment variable is found
}


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
    int count_size = 0;
    Path* path = getPath();
    char** pathToken;
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
        } else if (token[0] == '$') {

            char* path =  process_variables(token);
            pathToken = tokenise(path, &count_size);

        }
        else {
            strcpy(buffer, token);
        }

        int tokenSize = strlen(buffer);
        *head = malloc(tokenSize + 1); // +1 for the null terminator
        if (head == NULL) {
            unrecoverableError("Error -> unable to allocate memory program ending");
        }

        if (count_size > 0) {
            for (int i = 0; i < count_size; i++) {
                int tokenSize = strlen(pathToken[i]);
                *head = malloc(tokenSize + 1);
                strncpy(*head,pathToken[i],tokenSize);
                head++;
                *size = *size + 1;
            }

        } else {
            strncpy(*head, buffer, tokenSize);
            (*head)[tokenSize] = '\0'; // Null terminate the string
            head++;
            *size = *size + 1;

        }
        free(buffer); // Free the temporary buffer
        token = strtok(NULL, delims);

    }
    for (int i = 0; i < *size; i++) {
        tokenisedInput[i];
    }
    return tokenisedInput;

}

void destructTokenInput(char **tokenisedInput, int count) {
    char** free_buffer = tokenisedInput;
    for (int i = 0; i < count; i++){
        free(tokenisedInput[count]);
    }
    free(free_buffer);
}
