#ifndef DECOMPILER_ELFPARSER_H
#define DECOMPILER_ELFPARSER_H
#include <elf.h>
#include <iostream>
#include <vector>

class ElfParser {
public:
    explicit ElfParser(const std::vector<char> &d) : data(d) {
        if (data.size() < sizeof(Elf64_Ehdr)) {
            std::cerr << "File is not an ELF file";
            exit(1);
        }

        header = reinterpret_cast<const Elf64_Ehdr *>(data.data());

        validate();
    }

private:
    const std::vector<char> data;
    const Elf64_Ehdr *header;

    void validate() const {
        if (header->e_ident[EI_MAG0] != ELFMAG0 ||
            header->e_ident[EI_MAG1] != ELFMAG1 ||
            header->e_ident[EI_MAG2] != ELFMAG2 ||
            header->e_ident[EI_MAG3] != ELFMAG3) {
            std::cerr << "File is not an ELF file";
            exit(1);
        }

        if (header->e_ident[EI_CLASS] != ELFCLASS64) {
            std::cerr << "This decompiler only supports ARM64 (This file is not 64-bit)";
            exit(1);
        }

        if (header->e_machine != EM_AARCH64) {
            std::cerr << "This decompiler only supports aarch64 (This file is not aarch64)";
            exit(1);
        }
    }
};


#endif //DECOMPILER_ELFPARSER_H
