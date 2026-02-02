#include <filesystem>
#include <fstream>
#include <iostream>

#include "Elf.h"
#include "FileLoader.h"

#include <capstone/capstone.h>

#include "ControlFlowGraph.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: decompiler [filename]";
        return -1;
    }

    FileLoader file{argv[1]};
    Elf elf{file.getData()};

    auto *textSec = elf.findSection(".text");
    if (!textSec) {
        throw std::runtime_error("Couldn't find text section");
    }
    auto code = elf.getSectionData(textSec);

    csh handle;
    if (cs_open(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, &handle) != CS_ERR_OK) {
        std::cerr << "Failed to initialize Capstone\n";
        return -1;
    }
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    uint64_t mainAddr = elf.findSymbolAddress("main");
    if (mainAddr == 0) mainAddr = elf.header->e_entry;

    ControlFlowGraph cfg(handle, elf.getSectionData(textSec), textSec->sh_addr, mainAddr);
    const auto blocks = cfg.parse();

    std::cout << "Found " << blocks.size() << " Basic Blocks.\n";

    for (int i = 0; const auto &block: blocks) {
        std::cout << "Block " << ++i << ":\n";

        for (const auto &i: block.instructions) {
            std::cout << "0x" << std::hex << i.address << "\t" << i.mnemonic << " " << i.op_str << "\n";
        }
        std::cout << "\t\t(Jumps to:";
        for (auto s: block.successors) std::cout << "0x" << s << " ";
        std::cout << ")\n\n";
    }

    cs_close(&handle);

    return 0;
}
