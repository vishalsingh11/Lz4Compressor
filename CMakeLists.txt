cmake_minimum_required(VERSION 3.16)

project(compressor)

find_library(LZ4 lz4)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(compression)
add_subdirectory(compressionHC)

add_executable(compressor main.cpp)

target_link_libraries(compressor PUBLIC compression compressionHC "${LZ4}")