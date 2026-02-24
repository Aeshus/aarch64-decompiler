cmake_minimum_required(VERSION 4.1)
project(decompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(decompiler main.cpp
        Elf.cpp
        Elf.h
        cfg.cpp
        cfg.h
)

target_link_libraries(decompiler capstone)