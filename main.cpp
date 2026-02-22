#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Elf.h"

#define HELP_MSG "HELP: ./decompiler filename";
#define FILE_OPEN_FAIL_MSG "Failed to open file: "

int main(int const argc, char *const argv[]) {
    if (argc != 2) {
        std::cerr << HELP_MSG;
        return EXIT_FAILURE;
    }

    const char *path = argv[1];

    close(fd);
}
