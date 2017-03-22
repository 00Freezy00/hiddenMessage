cmake_minimum_required(VERSION 3.6)
project(hiddenMessage)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        hide.c)

add_executable(hide.c ${SOURCE_FILES} )


target_link_libraries(hide.c m)
