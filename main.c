#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/errno.h>

#include "output.h"
#include "utils.h"

// Constants for maximum path length and maximum number of threads
#define PATH_MAX 1024
#define MAX_THREAD 20

// Function to process environmental variables in the path
char* process_variables(char* path) {
    if (path == NULL) {
        return NULL;
    }
    // Check if path begins with '$', indicating an environment variable
    if (path[0] == '$') {
        const char* variable = getenv(path + 1);  // Retrieve the environment variable
        if (variable == NULL) {
            return NULL;  // Environment variable not found
        }
        char* buffer = malloc(strlen(variable) + 1);  // Allocate memory for the variable
        if (buffer == NULL) {
            return NULL;  // Memory allocation failed
        }
        strcpy(buffer, variable);
        return buffer;
    }
    return path; // Return the original path if no environment variable is found
}

// Thread function to print executable files in directories
void* printExe(void* dirs) {
    char* directories = dirs;
    int count = 0;
    char** tokenisedInput = tokenise(directories, &count); // Tokenise the input directory paths

    for (int i = 0; i < count; i++) {
        char *process_dir = process_variables(tokenisedInput[i]); // Process each directory path
        if (process_dir  == NULL) {
            destructTokenInput(tokenisedInput, count);
            recoverableError("Invalid path");
            pthread_exit(EXIT_FAILURE);
        }

        DIR* directory = opendir(process_dir); // Open the directory
        if (directory == NULL) {
            recoverableError("Invalid directory");
            destructTokenInput(tokenisedInput, count);
            pthread_exit(EXIT_FAILURE);
        }
        printf("-----------> OPENING DIRECTORY: %s\n", tokenisedInput[i]);
        errno = 0;  // Reset errno before calling readdir
        struct dirent* direct;
        while ((direct = readdir(directory)) != NULL) {
            char fullPath[PATH_MAX] = {};
            snprintf(fullPath, sizeof(fullPath), "%s/%s", process_dir, direct->d_name); // Construct the full path
            struct stat sb;
            // Check if the file is an executable
            if (stat(fullPath, &sb) == 0 && S_ISREG(sb.st_mode) && (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                printf("%s\n", direct->d_name); // Print the file name
            }
        }
        closedir(directory); // Close the directory
        if (strcmp(process_dir, dirs) != 0) {
            free(process_dir); // Free memory if it was dynamically allocated
        }
        if (errno != 0) {
            recoverableError("Unable to read directory address");
            closedir(directory);
            destructTokenInput(tokenisedInput, count);
            pthread_exit(EXIT_FAILURE);
        }



    }
    free(tokenisedInput); // Free the tokenised input array
    pthread_exit(EXIT_SUCCESS);
}

// Function to get the current working directory
char* getCwd() {
    char* directory = malloc(PATH_MAX);
    if (directory == NULL) {
        unrecoverableError("Error -> cannot allocate memory ending program");
    }
    getcwd(directory, PATH_MAX);
    return directory;
}

// Main function to create threads and execute printExe
int main(const int argc, char** argv) {
    if (argc == 1) {
        char* string = getCwd(); // Get current working directory if no arguments are passed
        printExe(string);
        free(string); // Free the string after use
        return EXIT_SUCCESS;
    }
    pthread_t thread_id[MAX_THREAD]; // Array to store thread IDs
    int count = 0;
    do {
        if (count > MAX_THREAD - 1) {
            unrecoverableError("Error -> Too many threads allocated ending program");
        }
        printf("---------------------------> STARTING THREAD: %d\n", count);
        const int thread_status = pthread_create(&thread_id[count], NULL, printExe, argv[count + 1]);
        if (thread_status != 0) {
            recoverableError("Error -> Cannot create thread");
        }
    } while (count < argc - 1); // Iterate over all input arguments except the program name
    count = 0;
    do {
        pthread_join(thread_id[count], NULL); // Wait for all threads to finish
        count++;
    } while (count < argc - 1);
    return EXIT_SUCCESS;
}
