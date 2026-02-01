#ifndef DECOMPILER_ELF_H
#define DECOMPILER_ELF_H

#include <vector>

#include "ElfHeader.h"
#include "ElfSection.h"
#include "FileLoader.h"

class Elf {
public:
    explicit Elf(std::span<const std::byte> data) : data(data) {
        header = ElfHeader{data};
        elf_class = header.elf_class;

        parseSections();
    }

private:
    ElfClass elf_class;
    ElfHeader header;
    std::span<const std::byte> data;
    std::vector<ElfSection> sections;

    void parseSections() {
        const auto offset = header.elf_section_header_offset;
        const auto size = header.elf_section_header_entry_size;
        const auto count = header.elf_section_header_entry_count;

        sections.reserve(count);

        for (int i = 0; i < count; i++) {
            const size_t o = offset + i * size;

            if (o + size > data.size_bytes()) break;

            ElfSection section{data.subspan(o, size), elf_class};

            sections.push_back(std::move(section));
        }

        auto stringIndex = header.elf_section_string_index;
        if (stringIndex > sections.size())
            throw std::runtime_error("There is no strings table");

        auto stringTable = sections[stringIndex];
        for (auto &sec: sections) {
            sec.name = getSectionName(stringTable, sec.name_index);
        }
    }

    std::string getSectionName(const ElfSection &stringTable, const uint64_t index) {
        const char *start = reinterpret_cast<const char *>(stringTable.data.data() + index);
        size_t maxLen = stringTable.data.size_bytes() - index;
        size_t len = strnlen(start, maxLen);
        return {start, len};
    }
};

#endif //DECOMPILER_ELF_H
