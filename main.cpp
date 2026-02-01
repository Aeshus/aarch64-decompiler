#include <filesystem>
#include <fstream>
#include <iostream>

#include "Elf.h"
#include "FileLoader.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: decompiler [filename]";
        return -1;
    }

    FileLoader file{argv[1]};
    Elf elf{file.getData()};

    std::cout << "Entry Point: 0x" << std::hex << elf.header->e_entry << "\n";

    auto* textSec = elf.findSection(".text");
    if (textSec) {
        auto code = elf.getSectionData(textSec);
        std::cout << "Found .text: " << code.size() << " bytes\n";
    }

    return 0;
}
