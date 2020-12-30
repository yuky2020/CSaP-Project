#the always fantastic cmake
    cmake_minimum_required(VERSION 3.19)

    project(CSaP_Project C)

set(CMAKE_C_STANDARD 11)
file(GLOB helloworld_SRC CONFIGURE_DEPENDS "*.h" "*.cpp")
set (CMAKE_CXX_FLAGS "-lrt -lm -lasound -ljack -pthread")
include_directories(audio)

add_executable(CSaP_Project
      audio/paex_record.c)
target_link_libraries( CSaP_Project  /audio/libportaudio.a)