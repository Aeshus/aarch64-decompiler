//
// Created by aeshus on 3/16/26.
//

#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define USAGE_MSG "usage:\n" \
    "decompiler [file]"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << USAGE_MSG;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
