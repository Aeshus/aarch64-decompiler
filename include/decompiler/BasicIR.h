//
// Created by aeshus on 3/22/26.
//

#ifndef DECOMPILER_BASICIR_H
#define DECOMPILER_BASICIR_H
#include <string>
#include <vector>
#include <optional>
#include <capstone/capstone.h>

#include "Elf.h"

enum class BasicOp {
    Add,
    Load,
};

// x0 = IADD(x1, x2)
// t = Dereference(x1)
// x1 = LOAD(t)
struct BasicIRInstruction {
    BasicOp operation;
    std::optional<std::string> first;
    std::optional<std::string> second;
    std::optional<std::string> operand;

    BasicIRInstruction(cs_insn instruction);
};

struct BasicIRInstructionBlock {
    std::vector<BasicIRInstruction> instructions;
    uint64_t address;

    BasicIRInstructionBlock(uint64_t address,
                            std::vector<BasicIRInstruction> instructions);
};


struct BasicIR {
    std::vector<BasicIRInstructionBlock> blocks;

    BasicIR(Elf elf);
};


#endif //DECOMPILER_BASICIR_H
