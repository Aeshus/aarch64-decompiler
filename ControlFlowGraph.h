#ifndef DECOMPILER_CONTROLFLOWGRAPH_H
#define DECOMPILER_CONTROLFLOWGRAPH_H

#include <deque>
#include <set>
#include <span>
#include <string>
#include <vector>
#include <capstone/capstone.h>

struct Instruction {
    uint64_t address;
    std::string mnemonic;
    std::string op_str;
};

struct BasicBlock {
    uint64_t start_addr;
    uint64_t end_addr;
    std::vector<Instruction> instructions;
    std::vector<uint64_t> successors;
};

class ControlFlowGraph {
public:
    explicit
    ControlFlowGraph(csh handle, std::span<const char> data,
                     uint64_t start, uint64_t entry) : handle(handle), data(data), start(start), entry(entry) {
    };

    std::vector<BasicBlock> parse() {
        std::deque<uint64_t> addresses;
        addresses.push_back(entry);

        std::set<uint64_t> visited;
        std::vector<BasicBlock> blocks;
        cs_insn *insn = cs_malloc(handle);

        while (!addresses.empty()) {
            uint64_t addr = addresses.front();
            addresses.pop_front();

            if (visited.contains(addr)) continue;
            visited.insert(addr);

            BasicBlock block{};
            block.start_addr = addr;

            if (addr < start) continue; // Safety
            uint64_t offset = addr - start;
            if (offset >= data.size()) continue;

            const uint8_t *codePtr = reinterpret_cast<const uint8_t *>(data.data() + offset);
            size_t codeSize = data.size() - offset;
            uint64_t instrAddr = addr;

            while (cs_disasm_iter(handle, &codePtr, &codeSize, &instrAddr, insn)) {
                block.instructions.push_back({
                    insn->address,
                    insn->mnemonic,
                    insn->op_str,
                });

                if (isTerminator(insn)) {
                    for (auto targets = getTargets(insn); auto t: targets) {
                        block.successors.push_back(t);
                        if (!visited.contains(t)) {
                            addresses.push_back(t);
                        }
                    }
                    break;
                }
            }

            block.end_addr = block.instructions.back().address;
            blocks.push_back(std::move(block));
        }

        cs_free(insn, 1);
        return blocks;
    }

private:
    csh handle;
    std::span<const char> data;
    uint64_t entry;
    uint64_t start;

    static bool isTerminator(const cs_insn *insn) {
        if (insn->id == ARM64_INS_BL) return false;

        for (int i = 0; i < insn->detail->groups_count; i++) {
            if (uint8_t group = insn->detail->groups[i];
                group == CS_GRP_JUMP || group == CS_GRP_RET || group == CS_GRP_BRANCH_RELATIVE)
                return true;
        }
        return false;
    }

    std::vector<uint64_t> getTargets(cs_insn *insn) {
        std::vector<uint64_t> targets;

        for (int i = 0; i < insn->detail->arm64.op_count; ++i) {
            auto &op = insn->detail->arm64.operands[i];
            if (op.type == ARM64_OP_IMM) {
                targets.push_back(op.imm);
            }
        }

        bool fallsThrough = true;

        if (insn->id == ARM64_INS_B) {
            if (insn->detail->arm64.cc == ARM64_CC_INVALID || insn->detail->arm64.cc == ARM64_CC_AL) {
                fallsThrough = false;
            }
        } else if (insn->id == ARM64_INS_RET) {
            fallsThrough = false;
        }

        if (fallsThrough) {
            targets.push_back(insn->address + 4);
        }

        return targets;
    }
};


#endif //DECOMPILER_CONTROLFLOWGRAPH_H
