#include <filesystem>
#include <fstream>
#include <iostream>
#include "FileLoader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: decompiler [filename]";
        return -1;
    }

    FileLoader file{argv[1]};

    return 0;
}
