cmake_minimum_required(VERSION 3.20)

project(cxx20
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

message("including test/doctest")
add_subdirectory(test/doctest)
