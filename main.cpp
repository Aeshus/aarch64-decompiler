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

    const int fd = open(path, O_RDONLY);
    if (fd == -1) {
        close(fd);
        std::cerr << FILE_OPEN_FAIL_MSG << path;
    }

    struct stat s{};
    fstat(fd, &s);
    if (fd == -1) {
        close(fd);
        std::cerr << FILE_OPEN_FAIL_MSG << path;
    }

    Elf elf{fd, s.st_size};

    std::unique_ptr<Elf64_Ehdr> header = elf.getHeader();

    close(fd);
}
