//
// Created by aeshus on 3/16/26.
//

#include "Elf.h"

bool Elf::is_elf() const {
    auto header = reinterpret_cast<Elf64_Ehdr *>(data);

    return header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] ==
           ELFMAG1
           && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] ==
           ELFMAG3;
}

bool Elf::is_64bit() const {
    auto header = reinterpret_cast<Elf64_Ehdr *>(data);

    return header->e_ident[EI_CLASS] == ELFCLASS64;
}

bool Elf::is_aarch64() const {
    auto header = reinterpret_cast<Elf64_Ehdr *>(data);

    return header->e_machine == EM_AARCH64;
}

char *Elf::addr_to_real_ptr(const Elf64_Addr addr) {
    for (const auto table = get_program_header_table(); const auto phdr:
         table) {
        if (phdr->p_type == PT_LOAD) {
            if (addr >= phdr->p_vaddr && addr < (
                    phdr->p_vaddr + phdr->p_memsz)) {
                const uint64_t offset = addr - phdr->p_vaddr + phdr->p_offset;
                return data + offset;
            }
        }
    }

    return nullptr;
}

char *Elf::get_entry_point() {
    const auto header = reinterpret_cast<Elf64_Ehdr *>(data);
    return addr_to_real_ptr(header->e_entry);
}

std::vector<Elf64_Phdr *> Elf::get_program_header_table() const {
    const auto header = reinterpret_cast<Elf64_Ehdr *>(data);

    std::vector<Elf64_Phdr *> v{};

    for (int i = 0; i < header->e_phnum; i++) {
        v.push_back(
            reinterpret_cast<Elf64_Phdr *>(
                data + header->e_phoff + header->e_phentsize * i));
    }

    return v;
}

std::vector<Elf64_Shdr *> Elf::get_section_header_table() const {
    const auto header = reinterpret_cast<Elf64_Ehdr *>(data);

    std::vector<Elf64_Shdr *> v{};

    for (int i = 0; i < header->e_shnum; i++) {
        v.push_back(
            reinterpret_cast<Elf64_Shdr *>(
                data + header->e_shoff + header->e_shentsize * i));
    }

    return v;
}

std::vector<char *> Elf::get_string_table() const {
    const auto header = reinterpret_cast<Elf64_Ehdr *>(data);
    const auto sections = get_section_header_table();

    auto strsection = sections.at(header->e_shstrndx);

    std::vector<char *> v{};

    auto start = data + strsection->sh_offset;
    auto end = start + strsection->sh_size;
    auto i = 0;
    while (start <= end) {
        if (*(start + (i++)) == '\0') {
            v.push_back(start);
            start = start + i;
            i = 1;
        }
    }

    return v;
}

std::vector<Elf64_Shdr *> Elf::get_executable_sections() const {
    std::vector<Elf64_Shdr *> copy;
    for (auto section : get_section_header_table()) {
        if (section->sh_flags != SHF_EXECINSTR)
            continue;

        if (section->sh_size != 0)
            continue;

        copy.push_back(section);
    }

    return copy;
}