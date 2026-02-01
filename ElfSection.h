#ifndef DECOMPILER_ELFSECTION_H
#define DECOMPILER_ELFSECTION_H

#include <cstddef>
#include <cstdint>
#include <span>

#include "ElfHeader.h"


class ElfSection {
public:
    uint32_t name_index;
    uint32_t type;
    uint64_t flags;
    uint64_t address;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t address_alignment;
    uint64_t entry_size;

    std::span<const std::byte> data;

    std::string name;

    explicit ElfSection(const std::span<const std::byte> data, ElfClass elf_class) : data(data) {
        Elf64_Shdr h;
        std::memcpy(&h, data.data(), sizeof(Elf64_Shdr));
        name_index = h.sh_name;
        type = h.sh_type;
        flags = h.sh_flags;
        address = h.sh_addr;
        offset = h.sh_offset;
        size = h.sh_size;
        link = h.sh_link;
        info = h.sh_info;
        address_alignment = h.sh_addralign;
        entry_size = h.sh_entsize;
    }
};


#endif //DECOMPILER_ELFSECTION_H
