cmake_minimum_required(VERSION 3.6)
project(hiddenMessage)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES1
        hide.c)

set(SOURCE_FILES2 unhide.c)

add_executable(hide.c supportLib.c ${SOURCE_FILES1} )
add_executable(unhide.c supportLib.c ${SOURCE_FILES2} )

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS}
        ${SDL2_IMG_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIX_INCLUDE_DIRS})

link_directories (${SDL2_LIBRARY_DIRS}
        ${SDL2_IMG_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
        ${SDL2_MIX_LIBRARY_DIRS})

target_link_libraries(hide.c m ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIX_LIBRARIES})
target_link_libraries(unhide.c m ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIX_LIBRARIES})

