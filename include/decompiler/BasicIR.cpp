//
// Created by aeshus on 3/22/26.
//

#include "BasicIR.h"

#include <stdexcept>
#include <capstone/capstone.h>
#include "Elf.h"

BasicIRInstruction::BasicIRInstruction(cs_insn instruction) {
    operation = BasicOp::Load;
}


BasicIRInstructionBlock::BasicIRInstructionBlock(uint64_t address,
                                                 std::vector<BasicIRInstruction>
                                                 instructions) : address(
    address), instructions(instructions) {
}

BasicIR::BasicIR(Elf elf) : blocks({}) {
    auto sections = elf.get_executable_sections();

    csh handle;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        throw std::runtime_error("Couldn't open capstone");
    }

    for (auto section: sections) {
        auto start = reinterpret_cast<uint8_t *>(section + sizeof(Elf64_Shdr));
        cs_insn *insn;

        size_t count = cs_disasm(handle, start, section->sh_size,
                                 section->sh_addr, 0,
                                 &insn);

        std::vector<cs_insn> ins{insn, insn + count};

        std::vector<BasicIRInstruction> v{};
        for (auto i: ins) {
            BasicIRInstruction is{i};
            v.push_back(is);
        }

        BasicIRInstructionBlock instructions{section->sh_addr, v};

        blocks.push_back(instructions);
        cs_free(insn, count);
    }

    cs_close(&handle);
}
