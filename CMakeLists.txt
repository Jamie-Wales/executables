cmake_minimum_required(VERSION 3.27)
project(executables C)

set(CMAKE_C_STANDARD 17)

add_executable(executables main.c
        path.c
        path.h
        output.c
        output.h
        utils.c
        utils.h)
