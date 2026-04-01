//
// Created by aeshus on 3/22/26.
//

#include "BasicIR.h"

#include <stdexcept>
#include <utility>
#include <cstdio>
#include <cinttypes>
#include <capstone/capstone.h>
#include "Elf.h"
#include <iomanip>

BasicIRInstruction::BasicIRInstruction(BasicOp op, std::optional<IROperand> d,
                                       std::optional<IROperand> s1,
                                       std::optional<IROperand> s2)
    : operation(op), dest(d), src1(s1), src2(s2) {
}

BasicIRInstructionBlock::BasicIRInstructionBlock(const cs_insn *instruction) {
    address = instruction->address;

    // We use detail to get a bunch of aarch64 specific stuff out
    // If we can't get it, we've hit a bad place.
    if (!instruction->detail)
        throw std::runtime_error("Cannot get detail from Capstone");

    cs_arm64 *arm64 = &instruction->detail->arm64;

    uint32_t temp = 0;
    auto new_vreg = [&] {
        return IROperand::create_virt_reg(temp++);
    };

    auto operand = [&](const cs_arm64_op &op) -> IROperand {
        switch (op.type) {
            case ARM64_OP_REG:
                return IROperand::create_phys_reg(op.reg);
            case ARM64_OP_IMM:
            case ARM64_OP_CIMM:
                return IROperand::create_imm(op.imm);
            case ARM64_OP_SYS:
                return IROperand::create_imm(op.sys);
            default: {
                throw std::runtime_error("Failed to parse operand");
            }
        }
    };

    auto addr = [&](const cs_arm64_op &op) -> IROperand {
        if (op.type == ARM64_OP_IMM) {
            return IROperand::create_imm(op.imm);
        }

        IROperand base_addr = new_vreg();

        // t# = base + offset
        instructions.emplace_back(BasicOp::Add,
                                  base_addr,
                                  IROperand::create_phys_reg(op.mem.base),
                                  IROperand::create_imm(op.mem.disp));

        return base_addr;
    };

    auto writeback = [&](const cs_arm64_op &op) {
        if (arm64->writeback && op.type == ARM64_OP_MEM) {
            instructions.emplace_back(BasicOp::Add,
                                      IROperand::create_phys_reg(
                                          op.mem.base),
                                      IROperand::create_phys_reg(
                                          op.mem.base),
                                      IROperand::create_imm(op.mem.disp));
        }
    };

    switch (instruction->id) {
        case ARM64_INS_NOP:
            instructions.emplace_back(BasicOp::Nop);
            break;

        case ARM64_INS_ADD:
            instructions.emplace_back(BasicOp::Add,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;
        case ARM64_INS_SUB:
            instructions.emplace_back(BasicOp::Sub,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;
        case ARM64_INS_AND:
            instructions.emplace_back(BasicOp::And,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;
        case ARM64_INS_LSL:
            instructions.emplace_back(BasicOp::Lsl,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;
        case ARM64_INS_LSR:
            instructions.emplace_back(BasicOp::Lsr,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;
        case ARM64_INS_ASR:
            instructions.emplace_back(BasicOp::Asr,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]),
                                      operand(arm64->operands[2]));
            break;

        case ARM64_INS_CMP: {
            IROperand tmp = new_vreg();
            instructions.emplace_back(BasicOp::Sub, tmp,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]));
            instructions.emplace_back(BasicOp::CmpEq,
                                      IROperand::create_flag(IRFlag::Z), tmp,
                                      IROperand::create_imm(0));
            instructions.emplace_back(BasicOp::CmpLt,
                                      IROperand::create_flag(IRFlag::N), tmp,
                                      IROperand::create_imm(0));
            break;
        }

        case ARM64_INS_MOV:
            instructions.emplace_back(BasicOp::Mov,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]));
            break;
        case ARM64_INS_ADRP:
            instructions.emplace_back(BasicOp::Adrp,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]));
            break;

        case ARM64_INS_LDR:
        case ARM64_INS_LDRB: {
            // TODO: need to handle the post indexing state
            // https://stackoverflow.com/questions/66107147/instruction-writeback-in-arm
            IROperand a = addr(arm64->operands[1]);
            instructions.emplace_back(BasicOp::Load,
                                      operand(arm64->operands[0]),
                                      a);
            writeback(arm64->operands[1]);
            break;
        }

        case ARM64_INS_STR:
        case ARM64_INS_STRB: {
            IROperand a = addr(arm64->operands[1]);
            instructions.emplace_back(BasicOp::Store, a,
                                      operand(arm64->operands[0]));
            writeback(arm64->operands[1]);
            break;
        }

        case ARM64_INS_STP: {
            IROperand base_addr = addr(arm64->operands[2]);
            instructions.emplace_back(BasicOp::Store, base_addr,
                                      operand(arm64->operands[0]));

            IROperand offset_addr = new_vreg();
            instructions.emplace_back(BasicOp::Add, offset_addr, base_addr,
                                      IROperand::create_imm(8));
            instructions.emplace_back(BasicOp::Store, offset_addr,
                                      operand(arm64->operands[1]));

            writeback(arm64->operands[2]);
            break;
        }

        case ARM64_INS_LDP: {
            IROperand base_addr = addr(arm64->operands[2]);
            instructions.emplace_back(BasicOp::Load,
                                      operand(arm64->operands[0]),
                                      base_addr);

            IROperand offset_addr = new_vreg();
            instructions.emplace_back(BasicOp::Add, offset_addr, base_addr,
                                      IROperand::create_imm(8));
            instructions.emplace_back(BasicOp::Load,
                                      operand(arm64->operands[1]),
                                      offset_addr);

            writeback(arm64->operands[2]);
            break;
        }

        case ARM64_INS_B:
            if (arm64->cc != ARM64_CC_INVALID && arm64->cc != ARM64_CC_AL) {
                IROperand cond_flag = IROperand::create_flag(IRFlag::Z);
                instructions.emplace_back(BasicOp::BranchCond,
                                          operand(arm64->operands[0]),
                                          cond_flag);
            } else {
                instructions.emplace_back(BasicOp::Branch,
                                          operand(arm64->operands[0]));
            }
            break;

        case ARM64_INS_BR:
            instructions.emplace_back(BasicOp::Branch,
                                      operand(arm64->operands[0]));
            break;

        case ARM64_INS_BL:
            instructions.emplace_back(BasicOp::Call,
                                      operand(arm64->operands[0]));
            break;

        case ARM64_INS_CBZ: {
            IROperand tmp_flag = new_vreg();
            instructions.emplace_back(BasicOp::CmpEq, tmp_flag,
                                      operand(arm64->operands[0]),
                                      IROperand::create_imm(0));
            instructions.emplace_back(BasicOp::BranchCond,
                                      operand(arm64->operands[1]), tmp_flag);
            break;
        }

        case ARM64_INS_TBNZ: {
            IROperand tmp_flag = new_vreg();
            instructions.emplace_back(BasicOp::TestBit, tmp_flag,
                                      operand(arm64->operands[0]),
                                      operand(arm64->operands[1]));
            instructions.emplace_back(BasicOp::BranchCond,
                                      operand(arm64->operands[2]), tmp_flag);
            break;
        }

        case ARM64_INS_RET:
            instructions.emplace_back(BasicOp::Ret);
            break;

        case ARM64_INS_PACIASP:
            instructions.emplace_back(BasicOp::Paciasp);
            break;
        case ARM64_INS_AUTIASP:
            instructions.emplace_back(BasicOp::Autiasp);
            break;
        case ARM64_INS_BTI:
            instructions.emplace_back(BasicOp::Bti);
            // Safe: Removed the parameter parsing entirely
            break;

        default:
            instructions.emplace_back(BasicOp::Unknown);
            break;
    }
}

BasicIR::BasicIR(const Elf &elf) : instruction_blocks({}) {
    auto sections = elf.get_executable_sections();
    csh handle;

    if (cs_open(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, &handle) != CS_ERR_OK) {
        throw std::runtime_error("Couldn't open capstone");
    }

    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    for (const auto section: sections) {
        const auto start = reinterpret_cast<uint8_t *>(
            elf.data + section->sh_offset);
        cs_insn *insn;

        const size_t count = cs_disasm(handle, start, section->sh_size,
                                       section->sh_addr, 0, &insn);

        instruction_blocks.reserve(instruction_blocks.size() + count);
        for (size_t j = 0; j < count; ++j) {
            instruction_blocks.emplace_back(&insn[j]);
        }
        cs_free(insn, count);
    }

    cs_close(&handle);
}
