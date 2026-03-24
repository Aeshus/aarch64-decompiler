cmake_minimum_required(VERSION 4.1)
project(decompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(decompiler
        include/decompiler/main.cpp
        include/decompiler/File.cpp
        include/decompiler/File.h
        include/decompiler/Elf.cpp
        include/decompiler/Elf.h
        include/decompiler/BasicIR.cpp
        include/decompiler/BasicIR.h
        include/decompiler/SSA.cpp
        include/decompiler/SSA.h
)

target_link_libraries(decompiler capstone)