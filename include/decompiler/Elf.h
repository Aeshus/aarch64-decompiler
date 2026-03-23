#pragma once

#include "File.h"
#include "elf.h"
#include <vector>

struct Elf : File {
    bool is_elf() const;

    bool is_64bit() const;

    bool is_aarch64() const;

    char *addr_to_real_ptr(Elf64_Addr addr);

    char *get_entry_point();

    std::vector<Elf64_Phdr *> get_program_header_table() const;

    std::vector<Elf64_Shdr *> get_section_header_table() const;

    std::vector<char *> get_string_table() const;

    std::vector<Elf64_Shdr *> get_executable_sections() const;
};

