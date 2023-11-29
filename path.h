//
// Created by Jamie Wales on 26/11/2023.
//

#ifndef EXECUTABLES_PATH_H
#define EXECUTABLES_PATH_H

// Struct to hold information about paths
typedef struct Path {
    char *currentDir; // Current working directory
    char *home;       // Home directory
} Path;

Path *getPath(); // Function to initialize and get a Path structure

void destructPath(Path *path); // Function to free resources associated with a Path structure
#endif  // EXECUTABLES_PATH_H