#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include "ElfHeader.h"
#include "FileLoader.h"

class Elf {
public:
    explicit Elf(std::filesystem::path &path) {
        FileLoader loader{path};
        ElfHeader header{loader.getData()};
    }
};

#endif //DECOMPILER_ELF_H
