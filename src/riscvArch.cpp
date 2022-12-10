#include "riscvArch.h"
#include "binaryninjacore.h"
#include "disassembler.h"
#include "lifter.h"
#include "disassembler.h"

BNEndianness riscvArch::GetEndianness() const {
    return endian;
}

size_t riscvArch::GetAddressSize() const {
    return addressSize;
}

// Responsible for disassembling instructions and feeding BN info for the CFG
bool
riscvArch::GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen, BinaryNinja::InstructionInfo &result) {
    Instruction res = Disassembler::disasm(data, addr);
    if (res.type == InstrType::Error || maxLen < 4) {
        result.length = 0;
        return false;
    }

    // Check for branches
    switch (res.mnemonic) {
        case InstrName::BEQ:
        case InstrName::BNE:
        case InstrName::BLT:
        case InstrName::BGE:
        case InstrName::BLTU:
        case InstrName::BGEU:
            result.AddBranch(BNBranchType::TrueBranch, res.imm + addr);
            result.AddBranch(BNBranchType::FalseBranch, addr + 4);
            break;
        case InstrName::J:
            result.AddBranch(BNBranchType::UnconditionalBranch, res.imm);
            break;
        case InstrName::RET:
            result.AddBranch(BNBranchType::FunctionReturn);
            break;
        default:
            break;
    }

    result.length = 4;
    return true;
}

// Provides the text that BN displays for disassembly view
bool riscvArch::GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len,
                                   std::vector<BinaryNinja::InstructionTextToken> &result) {
    Instruction res = Disassembler::disasm(data, addr);
    if (res.type == InstrType::Error) {
        len = 0;
        return false;
    }

#define PADDING_SIZE 6
    char padding[PADDING_SIZE + 1];
    memset(padding, 0x20, sizeof(padding));
    size_t mnemonicLen = strlen(instrNames[res.mnemonic]);
    if (mnemonicLen < PADDING_SIZE) {
        padding[PADDING_SIZE - mnemonicLen] = '\0';
    } else
        padding[PADDING_SIZE] = '\0';

    result.emplace_back(BNInstructionTextTokenType::InstructionToken, instrNames[res.mnemonic]);
    result.emplace_back(BNInstructionTextTokenType::TextToken, padding);
    result.emplace_back(BNInstructionTextTokenType::TextToken, " ");

    switch (res.type) {
        case Rtype: {
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");

            if (res.mnemonic == InstrName::SLLIW || res.mnemonic == InstrName::SLLI || res.mnemonic == InstrName::SRLI || res.mnemonic == InstrName::SRLIW || res.mnemonic == InstrName::SRAI || res.mnemonic == InstrName::SRAIW)
                result.emplace_back(BNInstructionTextTokenType::IntegerToken, std::to_string(res.rs2));
            else
                result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs2]);
            break;
        }
        case Itype: {
            switch (res.mnemonic) {
                case InstrName::RET:
                    // ret pseudo-instruction
                    break;
                case InstrName::LI:
                    // li pseudo-instruction
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::IntegerToken, std::to_string(res.imm));
                    break;
                case InstrName::LB:
                case InstrName::LH:
                case InstrName::LW:
                case InstrName::LBU:
                case InstrName::LHU:
                case InstrName::LWU:
                case InstrName::LD:
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::CodeRelativeAddressToken, std::to_string(res.imm));
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, "(");
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
                    result.emplace_back(BNInstructionTextTokenType::TextToken, ")");
                    break;
                case InstrName::JR:
                case InstrName::JALR: {
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::CodeRelativeAddressToken, std::to_string(res.imm));
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, "(");
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
                    result.emplace_back(BNInstructionTextTokenType::TextToken, ")");
                    break;
                }
                case InstrName::MV:
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
                    break;
                default:
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
                    result.emplace_back(BNInstructionTextTokenType::IntegerToken, std::to_string(res.imm));
                    break;
            }
            break;
        }
        case Stype: {
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs2]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            result.emplace_back(BNInstructionTextTokenType::CodeRelativeAddressToken, std::to_string(res.imm));
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, "(");
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
            result.emplace_back(BNInstructionTextTokenType::TextToken, ")");
            break;
        }
        case Btype: {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%llx", res.imm + addr);

            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs2]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            result.emplace_back(BNInstructionTextTokenType::PossibleAddressToken, buf);
            break;
        }
        case Utype: {
            result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
            result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            result.emplace_back(BNInstructionTextTokenType::IntegerToken, std::to_string(res.imm));
            break;
        }
        case Jtype: {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%llx", res.imm);
            if (res.mnemonic != InstrName::J) {
                result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rd]);
                result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, ", ");
            }
            result.emplace_back(BNInstructionTextTokenType::PossibleAddressToken, buf, res.imm);
            break;
        }
        case Error:
            return false;
    }
    len = 4;
    return true;
}

bool riscvArch::GetInstructionLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len,
                                         BinaryNinja::LowLevelILFunction &il) {
    liftToLowLevelIL(data, addr, len, il);
    len = 4;
    return true;
}

std::vector<uint32_t> riscvArch::GetFullWidthRegisters() {
    return GetAllRegisters();
}

std::vector<uint32_t> riscvArch::GetAllRegisters() {
    std::vector<uint32_t> result;
    for (int i = 0; i < 33; ++i) {
        result.push_back(i);
    }
    return result;
}

std::string riscvArch::GetRegisterStackName(uint32_t regStack) {
    return registerNames[Registers::sp];
}

BNRegisterInfo riscvArch::GetRegisterInfo(uint32_t reg) {
    switch (reg) {
        case Registers::Zero:
        case Registers::ra:
        case Registers::sp:
        case Registers::gp:
        case Registers::tp:
        case Registers::t0:
        case Registers::t1:
        case Registers::t2:
        case Registers::s0:
        case Registers::s1:
        case Registers::a0:
        case Registers::a1:
        case Registers::a2:
        case Registers::a3:
        case Registers::a4:
        case Registers::a5:
        case Registers::a6:
        case Registers::a7:
        case Registers::s2:
        case Registers::s3:
        case Registers::s4:
        case Registers::s5:
        case Registers::s6:
        case Registers::s7:
        case Registers::s8:
        case Registers::s9:
        case Registers::s10:
        case Registers::s11:
        case Registers::t3:
        case Registers::t4:
        case Registers::t5:
        case Registers::t6:
        case Registers::pc:
            return RegisterInfo(reg);
        default:
            return RegisterInfo(0);
    }
}

BNRegisterInfo riscvArch::RegisterInfo(uint32_t fullWidthReg) {
    BNRegisterInfo result{};
    result.fullWidthRegister = fullWidthReg;
    result.offset = 0;
    result.size = 8;
    result.extend = SignExtendToFullWidth;
    return result;
}

uint32_t riscvArch::GetStackPointerRegister() { return Registers::sp; }

riscvArch::riscvArch(const std::string &name, BNEndianness endian_) : Architecture(name) {
    endian = endian_;
}

size_t riscvArch::GetDefaultIntegerSize() const {
    return 8;
}

size_t riscvArch::GetMaxInstructionLength() const {
    return 4;
}

std::string riscvArch::GetRegisterName(uint32_t reg) {
    if (reg < 33)
        return {registerNames[reg]};
    else {
        std::string unReg("x");
        unReg += std::to_string(reg);
        return unReg;
    }
}

uint32_t riscvArch::GetLinkRegister() { return Registers::ra; }
