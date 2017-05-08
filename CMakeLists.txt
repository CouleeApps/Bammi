cmake_minimum_required(VERSION 3.7)
project(Bammi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall")

set(SOURCE_FILES main.cpp Board.cpp Board.h AI.cpp AI.h Screen.cpp Screen.h)
add_executable(Bammi ${SOURCE_FILES})

target_link_libraries(Bammi ncurses)