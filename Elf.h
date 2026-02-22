#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include <sys/mman.h>
#include <memory>
#include "elf.h"

class Elf {
    int const fd;
    long int const sz;

public:
    explicit Elf(int const fd, long int const sz) : fd(fd), sz(sz), data(nullptr) {
        data = static_cast<char *>(mmap(nullptr, sz, PROT_READ, MAP_SHARED, fd, 0));
        if (data == nullptr || data == MAP_FAILED) {
            std::cerr << "mmap failed";
            exit(2);
        }
    }

private:
    char *data;
};

#endif
