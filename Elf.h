#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include "ElfHeader.h"
#include "FileLoader.h"

class Elf {
public:
    explicit Elf(std::filesystem::path &path) {
        FileLoader loader{path};

        ElfHeader header{loader.getData().subspan(0, sizeof(Elf64_Ehdr))};
    }
};

#endif //DECOMPILER_ELF_H
