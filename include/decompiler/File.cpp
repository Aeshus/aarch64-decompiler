#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include "File.h"

File::File(char *path) : fd(-1), sb({}), data(nullptr) {
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Could not open file: " << path;
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &sb) == -1) {
        std::cerr << "Could not read size of file: " << path;
        close(fd);
        exit(EXIT_FAILURE);
    }

    data = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));;
    if (data == MAP_FAILED) {
        std::cerr << "Unable to map file to memory: " << path;
        close(fd);
        exit(EXIT_FAILURE);
    }
}

File::~File() {
    munmap(data, sb.st_size);
    close(fd);
}
