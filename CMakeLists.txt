cmake_minimum_required(VERSION 3.7)
project(Bammi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall")

set(SOURCE_FILES
        src/main.cpp
        src/Board.cpp
        src/Board.h
        src/AI.cpp
        src/AI.h
        src/CursesScreen.cpp
        src/CursesScreen.h
        src/CoutScreen.cpp
        src/CoutScreen.h
        src/Screen.cpp
    )
add_executable(Bammi ${SOURCE_FILES})

target_link_libraries(Bammi ncurses)