#ifndef DECOMPILER_ELFHEADER_H
#define DECOMPILER_ELFHEADER_H
#include <span>
#include <stdexcept>

#include "elf.h"

enum class Architecture {
    Class32,
    Class64,
};

enum class Endian {
    Little,
    Big,
};

enum class Version {
    Current,
};

enum class OsAbi {
    SysV,
    Linux,
    Arm,
    ArmEabi,
    Standalone,
};

enum class AbiVersion {
    Conforming,
};

enum class Type {
    Relocatable,
    Executable,
    Shared,
    Core
};

enum class Machine {
    Aarch64,
};

class ElfHeader {
public:
    explicit ElfHeader(std::span<char> span) {
        if (span.size_bytes() < sizeof(Elf64_Ehdr))
            throw std::runtime_error("The file is not big enough to fit an ELF header");

        header = reinterpret_cast<const Elf64_Ehdr *>(span.data());

        if (!checkMagicBytes())
            throw std::runtime_error("This file is not an ELF, the magic bytes are missing");
    }

    [[nodiscard]] bool checkMagicBytes() const {
        return header->e_ident[EI_MAG0] == ELFMAG0 &&
               header->e_ident[EI_MAG1] == ELFMAG1 &&
               header->e_ident[EI_MAG2] == ELFMAG2 &&
               header->e_ident[EI_MAG3] == ELFMAG3;
    }

    [[nodiscard]] Architecture getArchitecture() const {
        switch (header->e_ident[EI_CLASS]) {
            case ELFCLASS32:
                return Architecture::Class32;
            case ELFCLASS64:
                return Architecture::Class64;
            default:
                throw std::runtime_error("The ELF file has an invalid architecture");
        }
    }

    [[nodiscard]] Endian getEndian() const {
        switch (header->e_ident[EI_DATA]) {
            case ELFDATA2LSB:
                return Endian::Little;
            case ELFDATA2MSB:
                return Endian::Big;
            default:
                throw std::runtime_error("The ELF file has an invalid endianness");
        }
    }

    [[nodiscard]] Version getVersion() const {
        switch (header->e_ident[EI_VERSION]) {
            case EV_CURRENT:
                return Version::Current;
            default:
                throw std::runtime_error("The ELF has an invalid version");
        }
    }

    [[nodiscard]] OsAbi getOsAbi() const {
        switch (header->e_ident[EI_OSABI]) {
            case ELFOSABI_ARM:
                return OsAbi::Arm;
            case ELFOSABI_ARM_AEABI:
                return OsAbi::ArmEabi;
            case ELFOSABI_LINUX:
                return OsAbi::Linux;
            case ELFOSABI_SYSV:
                return OsAbi::SysV;
            case ELFOSABI_STANDALONE:
                return OsAbi::Standalone;
            default:
                throw std::runtime_error("The ELF has an invalid OS ABI");
        }
    }

    [[nodiscard]] AbiVersion getAbiVersion() const {
        switch (header->e_ident[EI_ABIVERSION]) {
            case EI_OSABI:
                return AbiVersion::Conforming;
            default:
                throw std::runtime_error("This ELF has an incompatible ABI version");
        }
    }

    [[nodiscard]] Type getType() const {
        switch (header->e_type) {
            case ET_REL:
                return Type::Relocatable;
            case ET_EXEC:
                return Type::Executable;
            case ET_DYN:
                return Type::Shared;
            case ET_CORE:
                return Type::Core;
            default:
                throw std::runtime_error("This ELF has an invalid type");
        }
    }

    [[nodiscard]] Machine getMachine() const {
        switch (header->e_machine) {
            case EM_AARCH64:
                return Machine::Aarch64;
            default:
                throw std::runtime_error("This ELF has an invalid machine");
        }
    }

    [[nodiscard]] unsigned long getEntry() const {
        return header->e_entry;
    }

    [[nodiscard]] unsigned long getProgramHeaderOffset() const {
        return header->e_phoff;
    }

    [[nodiscard]] unsigned long getSectionHeaderOffset() const {
        return header->e_shoff;
    }

    [[nodiscard]] unsigned int getFlags() const {
        return header->e_flags;
    }

    [[nodiscard]] unsigned short int getElfHeaderSize() const {
        return header->e_ehsize;
    }

    [[nodiscard]] unsigned short int getProgramHeaderSize() const {
        return header->e_phentsize;
    }

    [[nodiscard]] unsigned short int getProgramHeaderCount() const {
        return header->e_phnum;z
    }

private:
    const Elf64_Ehdr *header;
};

#endif //DECOMPILER_ELFHEADER_H
