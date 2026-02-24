#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include <elf.h>
#include <string.h>

inline bool verify_header(const char *elf) {
    return strncmp(ELFMAG, elf, SELFMAG) == 0;
}

inline bool verify_arch(char *elf) {
    // We can safely cast as alignment to the field we are looking for is indentical.
    auto d = reinterpret_cast<Elf64_Ehdr *>(elf);

    return d->e_machine == EM_AARCH64;
}

inline char *elf_ptr_to_virtual_ptr(char *elf, Elf64_Addr addr) {
    auto ehdr = reinterpret_cast<Elf64_Ehdr *>(elf);
    auto phdr = reinterpret_cast<Elf64_Phdr *>(elf + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr &segment = phdr[i];

        // We only have to check loaded data for the possible place the pointer is going to
        if (segment.p_type == PT_LOAD) {
            if (addr >= segment.p_vaddr && addr < (segment.p_vaddr + segment.p_memsz)) {
                uint64_t file_offset = addr - segment.p_vaddr + segment.p_offset;
                return elf + file_offset;
            }
        }
    }

    return nullptr;
}

inline char *find_entry_point(char *elf) {
    auto ehdr = reinterpret_cast<Elf64_Ehdr *>(elf);

    return elf_ptr_to_virtual_ptr(elf, ehdr->e_entry);
}

#endif //DECOMPILER_ELF_H
