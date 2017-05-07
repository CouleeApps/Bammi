cmake_minimum_required(VERSION 3.7)
project(Bammi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall")

set(SOURCE_FILES main.cpp)
add_executable(Bammi ${SOURCE_FILES})