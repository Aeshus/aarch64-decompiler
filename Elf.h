#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include "ElfHeader.h"
#include "FileLoader.h"

class Elf {
public:
    explicit Elf(FileLoader loader) {
        ElfHeader header{loader.getData()};
    }
};

#endif //DECOMPILER_ELF_H
