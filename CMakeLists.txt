cmake_minimum_required(VERSION 4.1)
project(decompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(decompiler
        include/decompiler/main.cpp
        include/decompiler/File.cpp
        include/decompiler/File.h
)

target_link_libraries(decompiler capstone)