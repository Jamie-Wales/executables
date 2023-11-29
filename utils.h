//
// Created by Jamie Wales on 27/11/2023.
//

#ifndef UTILS_H
#define UTILS_H
//Function to tokenise comma separated list assumes ", " is passed
char** tokenise(char* string, int* size);
//Function to open directory and print all exe files
void* printExe(void* dirs);
void destructTokenInput(char **tokenisedInput, int count);

#endif //UTILS_H
