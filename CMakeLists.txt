cmake_minimum_required(VERSION 3.15)
project(Lab5_2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Lab5_2
        CSVparser.cpp
        CSVparser.hpp
        HashTable.cpp)
