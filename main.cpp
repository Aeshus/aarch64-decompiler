#include <elf.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <vector>
#include "ElfLoader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: decompiler [filename]";
        return -1;
    }

    ElfLoader loader{argv[1]};

    return 0;
}
