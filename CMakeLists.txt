#the always fantastic cmake
    cmake_minimum_required(VERSION 3.19)

    project(CSaP_Project C)

set(CMAKE_C_STANDARD 11)

include_directories(audio)

add_executable(CSaP_Project
        audio/paex_record.c)
