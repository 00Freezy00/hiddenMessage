cmake_minimum_required(VERSION 3.6)
project(hiddenMessage)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        main.c)

add_executable(main.c ${SOURCE_FILES} )

target_link_libraries(main.c m)