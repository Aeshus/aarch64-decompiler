#pragma once

#include "File.h"
#include "elf.h"
#include <vector>

struct Elf : File {
    [[nodiscard]] bool is_elf() const;

    [[nodiscard]] bool is_64bit() const;

    [[nodiscard]] bool is_aarch64() const;

    char *addr_to_real_ptr(Elf64_Addr addr);

    char *get_entry_point();

    [[nodiscard]] std::vector<Elf64_Phdr *> get_program_header_table() const;

    [[nodiscard]] std::vector<Elf64_Shdr *> get_section_header_table() const;

    [[nodiscard]] std::vector<char *> get_string_table() const;

    [[nodiscard]] std::vector<Elf64_Shdr *> get_executable_sections() const;
};
