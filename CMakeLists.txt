cmake_minimum_required(VERSION 3.6)
project(hiddenMessage)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES1
        hide.c)

set(SOURCE_FILES2 unhide.c)

add_executable(hide.c supportLib.c ${SOURCE_FILES1} )
add_executable(unhide.c supportLib.c ${SOURCE_FILES2} )


target_link_libraries(hide.c m)
target_link_libraries(unhide.c m)