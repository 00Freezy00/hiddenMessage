cmake_minimum_required(VERSION 3.6)
project(FIT3042_Assignment1)

set(CMAKE_C_STANDARD 99)


set(SOURCE_FILES main.c)

add_executable(FIT3042_Assignment1 ${SOURCE_FILES})

target_link_libraries(FIT3042_Assignment1 m)