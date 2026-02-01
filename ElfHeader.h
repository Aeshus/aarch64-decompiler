#ifndef DECOMPILER_ELFHEADER_H
#define DECOMPILER_ELFHEADER_H

#include <cstring>
#include <span>
#include <stdexcept>
#include <variant>

#include "elf.h"

/**
 * ELF architecture/file class.
 */
enum class ElfClass {
    /**
     * Invalid class.
     */
    ClassNone = ELFCLASSNONE,

    /**
     * 32-bit objects.
     */
    Class32 = ELFCLASS32,

    /**
     * 64-bit objects.
     */
    Class64 = ELFCLASS64,
};

/**
 * ELF data encoding.
 */
enum class ElfData {
    /**
     * Invalid data.
     */
    DataNone = ELFDATANONE,

    /**
     * Little endian, 2's complement.
     */
    DataLittleEndian = ELFDATA2LSB,

    /**
     * Big endian, 2's complement.
     */
    DataBigEndian = ELFDATA2MSB,
};

/**
 * ELF file version.
 */
enum class ElfVersion {
    /**
     * Invalid version.
     */
    VersionNone = EV_NONE,

    /**
     * Current version.
     */
    VersionCurrent = EV_CURRENT,
};

/**
 * ELF Operating System ABI.
 */
enum class ElfOSABI {
    /**
     * Unix System V ABI, or "None".
     */
    AbiSysV = ELFOSABI_SYSV,

    /**
     * Linux ABI, or "GNU" ABI (GNU ELF extensions).
     */
    AbiLinux = ELFOSABI_LINUX,

    /**
     * ARM EABI.
     */
    ABIArmEABI = ELFOSABI_ARM_AEABI,

    /**
     * ARM ABI.
     */
    ABIArm = ELFOSABI_ARM,

    /**
     * Standalone (embedded) application.
     */
    ABIStandalone = ELFOSABI_STANDALONE,
};

/**
 * ELF machine/ISA.
 */
enum class ElfMachine {
    /**
     * No machine.
     */
    MachineNone = EM_NONE,

    /**
     * AMD64 / x86_64.
     */
    MachineAMD64 = EM_X86_64,

    /**
     * ARM (32-bit).
     */
    MachineARM = EM_ARM,

    /**
     * ARM AARCH64.
     */
    MachineAARCH64 = EM_AARCH64,
};

class ElfHeader {
public:
    ElfClass elf_class;
    ElfData elf_data;
    ElfVersion elf_version;
    ElfOSABI elf_os_abi;
    unsigned char elf_abi_version;

    ElfMachine elf_machine;
    uint64_t elf_entry;
    uint64_t elf_program_header_offset;
    uint64_t elf_section_header_offset;

    /**
     * CPU/Machine flags, not handled yet.
     */
    uint32_t elf_flags;

    /**
     * ELF header size (in bytes)
     */
    uint16_t elf_header_size;

    /**
     * The size of each entry in the program header table.
     */
    uint16_t elf_program_header_entry_size;

    /**
     * The count of entries in the program header table.
     */
    uint16_t elf_program_header_entry_count;

    /**
     * The size of the entry in the section header table.
     */
    uint16_t elf_section_header_entry_size;

    /**
     * The count of entries in the section header table.
     */
    uint16_t elf_section_header_entry_count;

    /**
     * The index in the section header table that points to the string section.
     */
    uint16_t elf_section_string_index;

    explicit ElfHeader(std::span<const std::byte> data) {
        // The struct sizes are different between the 32-bit and 64-bit variants,
        // but the e_ident stuff is constant.
        if (data.size_bytes() < sizeof(unsigned char) * EI_NIDENT) {
            throw std::runtime_error("File is not large enough to fit an ELF.");
        }

        if (static_cast<int>(data[EI_MAG0]) != ELFMAG0 ||
            static_cast<int>(data[EI_MAG1]) != ELFMAG1 ||
            static_cast<int>(data[EI_MAG2]) != ELFMAG2 ||
            static_cast<int>(data[EI_MAG3]) != ELFMAG3) {
            throw std::runtime_error("File lacks the ELF magic bytes.");
        }

        const auto arch = static_cast<int>(data[EI_CLASS]);
        if (arch == ELFCLASS32) {
            throw std::runtime_error("32-bit ELFs are not supported");
        }

        if (data.size_bytes() < sizeof(Elf64_Ehdr))
            throw std::runtime_error("The file is not large enough to fit a 64-bit ELF file");

        Elf64_Ehdr h;
        header = h;

        parse();
    }

private:
    Elf64_Ehdr header{};

    void parse() {
        // Identifiers
        elf_class = static_cast<ElfClass>(header.e_ident[EI_CLASS]);
        elf_data = static_cast<ElfData>(header.e_ident[EI_DATA]);
        elf_version = static_cast<ElfVersion>(header.e_ident[EI_VERSION]);
        elf_os_abi = static_cast<ElfOSABI>(header.e_ident[EI_OSABI]);
        elf_abi_version = header.e_ident[EI_ABIVERSION];

        // Rest of header
        elf_machine = static_cast<ElfMachine>(header.e_machine);
        elf_entry = header.e_entry;
        elf_program_header_offset = header.e_phoff;
        elf_section_header_offset = header.e_shoff;
        elf_flags = header.e_flags;
        elf_program_header_entry_size = header.e_phentsize;
        elf_program_header_entry_count = header.e_phnum;
        elf_section_header_entry_size = header.e_shentsize;
        elf_section_header_entry_count = header.e_shnum;
        elf_section_string_index = header.e_shstrndx;
    }
};

#endif //DECOMPILER_ELFHEADER_H
