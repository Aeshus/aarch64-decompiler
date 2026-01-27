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
    HpUX,
    NetBSD,
    Linux,
    Solaris,
    Irix,
    FreeBSD,
    Tru64,
    Arm,
    OpenBSD,
    Modesto,
    Aix,
    ArmEabi,
    Standalone,
};

enum class AbiVersion {
    Conforming,
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
            case ELFOSABI_AIX:
                return OsAbi::Aix;
            case ELFOSABI_ARM:
                return OsAbi::Arm;
            case ELFOSABI_ARM_AEABI:
                return OsAbi::ArmEabi;
            case ELFOSABI_FREEBSD:
                return OsAbi::FreeBSD;
            case ELFOSABI_HPUX:
                return OsAbi::HpUX;
            case ELFOSABI_IRIX:
                return OsAbi::Irix;
            case ELFOSABI_MODESTO:
                return OsAbi::Modesto;
            case ELFOSABI_LINUX:
                return OsAbi::Linux;
            case ELFOSABI_NETBSD:
                return OsAbi::NetBSD;
            case ELFOSABI_OPENBSD:
                return OsAbi::OpenBSD;
            case ELFOSABI_SOLARIS:
                return OsAbi::Solaris;
            case ELFOSABI_SYSV:
                return OsAbi::SysV;
            case ELFOSABI_TRU64:
                return OsAbi::Tru64;
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



private:
    const Elf64_Ehdr *header;
};

#endif //DECOMPILER_ELFHEADER_H
