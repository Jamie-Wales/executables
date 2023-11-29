#include "path.h"
#include <stdlib.h>
#include <string.h>
#include "output.h"
#define BUFF_SIZE 1024



// Function to initialize and return a Path structure with current and home directories
Path* getPath() {
    char path[BUFF_SIZE];
    // Fetch the HOME environment variable
    strcpy(path, getenv("HOME"));

    // Allocate memory for the Path structure
    Path *output = malloc(sizeof(Path));
    if (output == NULL) {
        unrecoverableError("Memory allocation failure for Path");
        return NULL; // Return NULL in case of allocation failure
    }

    // Allocate memory and copy the HOME path to currentDir and home fields
    output->currentDir = (char*)calloc(strlen(path) + 1, sizeof(char));
    output->home = (char *)calloc(strlen(path) + 1, sizeof(char));
    if (output->currentDir == NULL || output->home == NULL) {
        unrecoverableError("Memory allocation failure for directory paths");
        free(output->currentDir); // Free if one allocation succeeded but the other failed
        free(output->home);
        free(output);
        return NULL;
    }

    strcpy(output->currentDir, path);
    strcpy(output->home, path);

    return output;
}
// Function to free resources associated with a Path structure

void destructPath(Path *path) {
    free(path->home);
    free(path->currentDir);
    free(path);
}
