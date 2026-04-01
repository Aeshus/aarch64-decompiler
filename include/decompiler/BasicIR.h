//
// Created by aeshus on 3/22/26.
//

#ifndef DECOMPILER_BASICIR_H
#define DECOMPILER_BASICIR_H

#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <capstone/capstone.h>

#include "Elf.h"

enum class BasicOp {
    Nop,
    Add,
    Sub,
    And,
    Lsl,
    Lsr,
    Asr,
    Mov,
    Branch,
    BranchCond,
    Call,
    Ret,
    Load,
    Store,
    SetFlag,
    Adrp,
    Paciasp,
    Autiasp,
    Bti,
    CmpEq,
    CmpNe,
    CmpLt,
    CmpGt,
    TestBit,
    Unknown
};

enum class IROpType {
    PhysicalReg,
    VirtualReg,
    Immediate,
    Label,
    Flag
};

enum class IRFlag { Z, N, C, V };

struct IROperand {
    IROpType type;
    std::variant<arm64_reg, uint32_t, uint64_t, IRFlag> value;

    uint32_t ssa_version = 0;

    static IROperand create_phys_reg(arm64_reg reg) {
        return {IROpType::PhysicalReg, reg, 0};
    }

    static IROperand create_virt_reg(uint32_t v_reg_id) {
        return {IROpType::VirtualReg, v_reg_id, 0};
    }

    static IROperand create_imm(uint64_t imm) {
        return {IROpType::Immediate, imm, 0};
    }

    static IROperand create_label(uint64_t addr) {
        return {IROpType::Label, addr, 0};
    }

    static IROperand create_flag(IRFlag flag) {
        return {IROpType::Flag, flag, 0};
    }
};

struct BasicIRInstruction {
    BasicOp operation;
    std::optional<IROperand> dest;
    std::optional<IROperand> src1;
    std::optional<IROperand> src2;

    BasicIRInstruction(BasicOp op,
                       std::optional<IROperand> d = std::nullopt,
                       std::optional<IROperand> s1 = std::nullopt,
                       std::optional<IROperand> s2 = std::nullopt);
};

struct BasicIRInstructionBlock {
    uint64_t address;
    std::vector<BasicIRInstruction> instructions;

    explicit BasicIRInstructionBlock(const cs_insn *instruction);
};

struct BasicIR {
    std::vector<BasicIRInstructionBlock> instruction_blocks;

    explicit BasicIR(const Elf &elf);

    void print() const;
};

#endif //DECOMPILER_BASICIR_H
