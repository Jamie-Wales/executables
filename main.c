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
#define BUFF_SIZE  2048
#define MAX_THREAD 20


// Thread function to print executable files in directories
void* printExe(void* dirs) {
    char* directories = (char*)dirs;
    int count = 0;
    char** tokenisedInput = tokenise(directories, &count); // Tokenise the input directory paths

    for (int i = 0; i < count; i++) {
        DIR* directory = opendir(tokenisedInput[i]);
        if (directory == NULL) {
            char errorBuf[BUFF_SIZE] = "Invalid directory ";
            recoverableError(strcat(errorBuf, tokenisedInput[i]));
            continue;
        }
        printf("-----------> OPENING DIRECTORY: %s\n", tokenisedInput[i]);
        errno = 0;  // Reset errno before calling readdir
        struct dirent* direct;
        while ((direct = readdir(directory)) != NULL) {
            char fullPath[BUFF_SIZE] = {};
            snprintf(fullPath, sizeof(fullPath), "%s/%s", tokenisedInput[i], direct->d_name); // Construct the full path
            struct stat sb;
            // Check if the file is an executable
            if (stat(fullPath, &sb) == 0 && S_ISREG(sb.st_mode) && (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                printf("%s\n", direct->d_name); // Print the file name
            }
        }
        closedir(directory); // Close the directory
        if (errno != 0) {
            recoverableError("Unable to read directory address");
            destructTokenInput(tokenisedInput, count);
            pthread_exit(EXIT_FAILURE);
        }
    }
    destructTokenInput(tokenisedInput, count);
    pthread_exit(EXIT_SUCCESS);
}


// Function to get the current working directory
char* getCwd() {
    char* directory = malloc(BUFF_SIZE);
    if (directory == NULL) {
        unrecoverableError("Error -> cannot allocate memory ending program");
    }
    getcwd(directory, BUFF_SIZE);
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

        int thread_status = pthread_create(&thread_id[count], NULL, printExe, argv[count + 1]);
        printf("---------------------------> STARTING THREAD: %d\n", count);
        if (thread_status != 0) {
            recoverableError("Error -> Cannot create thread");
        }
        count++;
    } while (count < argc - 1); // Iterate over all input arguments except the program name
    count = 0;
    do {
        pthread_join(thread_id[count], NULL); // Wait for all threads to finish
        count++;
    } while (count < argc - 1);
    return EXIT_SUCCESS;
}
