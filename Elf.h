#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include <cstring>
#include <elf.h>
#include <span>
#include <stdexcept>

class Elf {
public:
    const Elf64_Ehdr *header;
    std::span<const char> data;

    explicit Elf(std::span<const char> data) : data(data) {
        if (data.size() < sizeof(Elf64_Ehdr))
            throw std::runtime_error("File too small");

        header = reinterpret_cast<const Elf64_Ehdr *>(data.data());

        if (header->e_ident[EI_MAG0] != ELFMAG0 ||
            header->e_ident[EI_MAG1] != ELFMAG1 ||
            header->e_ident[EI_MAG2] != ELFMAG2 ||
            header->e_ident[EI_MAG3] != ELFMAG3) {
            throw std::runtime_error("Invalid ELF magic bytes");
        }

        if (header->e_machine != EM_AARCH64)
            throw std::runtime_error("Not AArch64");
    }

    [[nodiscard]] std::span<const Elf64_Shdr> getSections() const {
        auto *start = reinterpret_cast<const Elf64_Shdr *>(data.data() + header->e_shoff);
        return {start, header->e_shnum};
    }

    [[nodiscard]] const char *getStringTable() const {
        auto sections = getSections();
        auto &strTabSec = sections[header->e_shstrndx];
        return data.data() + strTabSec.sh_offset;
    }

    [[nodiscard]] const Elf64_Shdr *findSection(std::string_view name) const {
        auto sections = getSections();
        const char *strTab = getStringTable();

        for (const auto &sec: sections) {
            if (name == (strTab + sec.sh_name)) {
                return &sec;
            }
        }
        return nullptr;
    }

    std::span<const char> getSectionData(const Elf64_Shdr *sec) const {
        if (!sec) return {};
        if (sec->sh_type == SHT_NOBITS) return {};

        return data.subspan(sec->sh_offset, sec->sh_size);
    }

    [[nodiscard]] uint64_t findSymbolAddress(const char *name) const {
        const Elf64_Shdr *symTab = findSection(".symtab");
        const Elf64_Shdr *strTab = findSection(".strtab");

        if (!symTab || !strTab) return 0;

        auto syms = reinterpret_cast<const Elf64_Sym *>(data.data() + symTab->sh_offset);
        const char *strs = data.data() + strTab->sh_offset;
        size_t count = symTab->sh_size / sizeof(Elf64_Sym);

        for (size_t i = 0; i < count; i++) {
            const char *symName = strs + syms[i].st_name;
            if (strcmp(symName, name) == 0) {
                return syms[i].st_value;
            }
        }
        return 0;
    }
};


#endif //DECOMPILER_ELF_H
