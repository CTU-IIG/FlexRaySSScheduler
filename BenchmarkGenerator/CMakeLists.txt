cmake_minimum_required(VERSION 3.2)
project(BenchmarkGenerator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/jenik/Dropbox/ClionProjects/BenchmarkGenerator/Binary/")

set(SOURCE_FILES
    GDS.h
    Generator.cpp
    Generator.h
    Utility.cpp
    Utility.h)

add_executable(BenchmarkGenerator ${SOURCE_FILES})