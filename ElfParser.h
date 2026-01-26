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

    void parseSections() const {
        for (int i = 0; i < header->e_shnum; i++) {
            const auto offset = header->e_shoff + (i * header->e_shentsize);
            auto sectionHeader{reinterpret_cast<const Elf64_Shdr *>(&data[offset])};
            auto name = getSectionName(sectionHeader->sh_name);

            std::string nameStr = (name) ? name : "<null>";
            std::cout << "Found " << nameStr << " at offset 0x" << std::hex << sectionHeader->sh_offset << "\n";
            std::cout << std::dec;
        }
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

    [[nodiscard]] const char *getSectionName(int nameOffset) const {
        const auto offset = header->e_shoff + (header->e_shstrndx * header->e_shentsize);
        auto stringHeader{reinterpret_cast<const Elf64_Shdr *>(&data[offset])};
        auto d{reinterpret_cast<const char *>(&data[stringHeader->sh_offset])};
        return &d[nameOffset];
    }

};


#endif //DECOMPILER_ELFPARSER_H
