#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <capstone/capstone.h>

#include "Elf.h"

#define HELP_MSG "HELP: ./decompiler filename";
#define FILE_OPEN_FAIL_MSG "Failed to open file: "

int main(int const argc, char *const argv[]) {
    if (argc != 2) {
        std::cerr << HELP_MSG;
        return EXIT_FAILURE;
    }

    const char *path = argv[1];

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Cannot read file: " << path;
        return EXIT_FAILURE;
    }

    struct stat sb{};
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Cannot read file size of: " << path;
        close(fd);
        return EXIT_FAILURE;
    }

    const auto data = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0));;
    if (data == MAP_FAILED) {
        std::cerr << "Unable to map file to memory: " << path;
        close(fd);
        return EXIT_FAILURE;
    }

    if (!verify_header(data)) {
        std::cerr << "File is not an ELF: " << path;
        munmap(data, sb.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    if (!verify_arch(data)) {
        std::cerr << "ELF is not aarch64: " << path;
        munmap(data, sb.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    // Where the code entry point is actually located
    char* entry_point = find_entry_point(data);
    // Where the loaded ELF's virtual memory addr would be
    auto ehdr = reinterpret_cast<Elf64_Ehdr *>(data);
    uint64_t address = ehdr->e_entry;

    csh handle;
    cs_insn *insn;

    if (cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle) != CS_ERR_OK) {
        std::cerr << "Unable to dissassemble: " << path << '\n';
        munmap(data, sb.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    insn = cs_malloc(handle);
    auto code = reinterpret_cast<const uint8_t *>(entry_point);
    size_t size = sb.st_size - (entry_point - data);

    while (cs_disasm_iter(handle, &code, &size, &address, insn)) {
        std::cout << "0x" << std::hex << insn->address << ":\t"
                  << insn->mnemonic << "\t\t" << insn->op_str << '\n';
    }

    cs_free(insn, 1);
    cs_close(&handle);

    munmap(data, sb.st_size);
    return EXIT_SUCCESS;
}
