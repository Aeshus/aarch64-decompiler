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

class ElfHeader {
public:
    ElfClass elf_class;
    ElfData elf_data;
    ElfVersion elf_version;
    ElfOSABI elf_os_abi;
    uint8_t elf_abi_version;

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
            if (data.size_bytes() < sizeof(Elf32_Ehdr))
                throw std::runtime_error("The file is not large enough to fit a 32-bit ELF file");

            Elf32_Ehdr h;
            std::memcpy(&h, data.data(), sizeof(Elf32_Ehdr));
            header = h;
        } else if (arch == ELFCLASS64) {
            if (data.size_bytes() < sizeof(Elf64_Ehdr))
                throw std::runtime_error("The file is not large enough to fit a 64-bit ELF file");

            Elf64_Ehdr h;
            std::memcpy(&h, data.data(), sizeof(Elf64_Ehdr));
            header = h;
        } else {
            throw std::runtime_error("File architecture class is unknown.");
        }

        parse();
    }

private:
    std::variant<Elf64_Ehdr, Elf32_Ehdr> header{};

    uint8_t getIdentData(int index) {
        return std::visit([index](const auto &hdr) {
            return hdr.e_ident[index];
        }, header);
    }

    void parse() {
        // Identifiers
        elf_class = static_cast<ElfClass>(getIdentData(EI_CLASS));
        elf_data = static_cast<ElfData>(getIdentData(EI_DATA));
        elf_version = static_cast<ElfVersion>(getIdentData(EI_VERSION));
        elf_os_abi = static_cast<ElfOSABI>(getIdentData(EI_OSABI));
        elf_abi_version = getIdentData(EI_ABIVERSION);
    }

    [[nodiscard]] bool is64bit() const {
        return std::holds_alternative<Elf64_Ehdr>(header);
    }
};

#endif //DECOMPILER_ELFHEADER_H
