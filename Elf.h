#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include <memory>

#include "elf.h"

class Elf {
    int const fd;
    long int const sz;

public:
    explicit Elf(int const fd, long int const sz) : fd(fd), sz(sz) {
    }

    std::unique_ptr<Elf64_Ehdr> getHeader() {
        if (sz < sizeof(Elf64_Ehdr))
            return nullptr;

        auto header = std::make_unique<Elf64_Ehdr>();

        if (read(fd, header.get(), sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
            return nullptr;
        }

        return header;
    }

    // What API do I need?
    // Find section
    // Get elf header
    // Get program header
    // Search up symbols from symbol table
};

#endif //DECOMPILER_ELF_H
