#include "riscvArch.h"
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

    // Check for func return
    if (res.mnemonic == InstrName::RET)
        result.AddBranch(BNBranchType::FunctionReturn);

    // Check for branches
    switch (res.mnemonic) {
        case InstrName::BEQ:
        case InstrName::BNE:
        case InstrName::BLT:
        case InstrName::BGE:
        case InstrName::BLTU:
        case InstrName::BGEU:
            result.AddBranch(BNBranchType::TrueBranch, res.imm, nullptr, false);
            result.AddBranch(BNBranchType::FalseBranch, addr + 4, nullptr, false);
            break;
        case InstrName::J:
            result.AddBranch(BNBranchType::UnconditionalBranch, res.imm);
            break;
        case InstrName::JAL:
        case InstrName::JALR:
            result.AddBranch(BNBranchType::CallDestination, res.imm);
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

            if (res.mnemonic == InstrName::SLLIW)
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
                case InstrName::JALR:
                    result.emplace_back(BNInstructionTextTokenType::CodeRelativeAddressToken, std::to_string(res.imm));
                    result.emplace_back(BNInstructionTextTokenType::OperandSeparatorToken, "(");
                    result.emplace_back(BNInstructionTextTokenType::RegisterToken, registerNames[res.rs1]);
                    result.emplace_back(BNInstructionTextTokenType::TextToken, ")");
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
            sprintf(buf, "0x%llx", res.imm);

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
            sprintf(buf, "0x%llx", res.imm);
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
    if (!liftToLowLevelIL(data, addr, len, il))
        il.AddInstruction(il.Unimplemented());
    len = 4;
    return true;
}

std::vector<uint32_t> riscvArch::GetFullWidthRegisters() {
    return GetAllRegisters();
}

std::vector<uint32_t> riscvArch::GetAllRegisters() {
    std::vector<uint32_t> result = {0};
    for (int i = 0; i < 33; ++i) {
        result.push_back(i);
    }
    return result;
}

BNRegisterInfo riscvArch::GetRegisterInfo(uint32_t reg) {
    switch (reg) {
        case Registers::Zero:
        case Registers::Ra:
        case Registers::Sp:
        case Registers::Gp:
        case Registers::Tp:
        case Registers::T0:
        case Registers::T1:
        case Registers::T2:
        case Registers::S0:
        case Registers::S1:
        case Registers::A0:
        case Registers::A1:
        case Registers::A2:
        case Registers::A3:
        case Registers::A4:
        case Registers::A5:
        case Registers::A6:
        case Registers::A7:
        case Registers::S2:
        case Registers::S3:
        case Registers::S4:
        case Registers::S5:
        case Registers::S6:
        case Registers::S7:
        case Registers::S8:
        case Registers::S9:
        case Registers::S10:
        case Registers::S11:
        case Registers::T3:
        case Registers::T4:
        case Registers::T5:
        case Registers::T6:
        case Registers::Pc:
            return RegisterInfo(reg, 8);
    }
    return RegisterInfo(0, 0);
}

BNRegisterInfo riscvArch::RegisterInfo(uint32_t fullWidthReg, size_t size) {
    BNRegisterInfo result{};
    result.fullWidthRegister = fullWidthReg;
    result.offset = 0;
    result.size = size;
    result.extend = NoExtend;
    return result;
}

uint32_t riscvArch::GetStackPointerRegister() {
    return Registers::Sp;
}

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
    if (reg < 32)
        return {registerNames[reg]};
    else
        return {"unknown reg"};
}
