//
// Created by aeshus on 3/16/26.
//

#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "Elf.h"

#define USAGE_MSG "usage:\n" \
    "decompiler [file]"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << USAGE_MSG;
        return EXIT_FAILURE;
    }

    Elf elf{File(argv[1])};

    if (!elf.is_elf()) {
        std::cerr << "File is not an ELF: " << argv[1];
        return EXIT_FAILURE;
    }

    if (!elf.is_64bit()) {
        std::cerr << "ELF file must be 64-bit: " << argv[1];
        return EXIT_FAILURE;
    }

    if (!elf.is_aarch64()) {
        std::cerr << "ELF file must be in aarch64 architecture: " << argv[1];
        return EXIT_FAILURE;
    }

    auto strings = elf.get_string_table();
    for (char *string : strings) {
        std::cout << string << "\n";
    }

    return EXIT_SUCCESS;
}
