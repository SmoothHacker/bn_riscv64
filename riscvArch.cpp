#include "riscvArch.h"
#include "disassembler.h"

BNEndianness riscvArch::GetEndianness() const {
    return BigEndian;
}

size_t riscvArch::GetAddressSize() const {
    return addressSize;
}

// Responsible for disassembling instructions and feeding BN info for the CFG
bool
riscvArch::GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen, BinaryNinja::InstructionInfo &result) {
    Disassembler disassembler;

    Instruction res = disassembler.disasm(data, addr, maxLen);
    result.length = 1;
    return true;
}

// Provides the text that BN displays for disassembly view
bool riscvArch::GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len,
                                   std::vector<BinaryNinja::InstructionTextToken> &result) {
    Disassembler disassembler;
    Instruction res = disassembler.disasm(data, addr, 4);

    switch (res.type) {
        case Rtype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, %s, %s", res.mnemonic.c_str(), registerNames[res.rd], registerNames[res.rs1], registerNames[res.rs2]);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
        case Itype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, 0x%llu(%s)", res.mnemonic.c_str(), registerNames[res.rd], res.imm, registerNames[res.rs1]);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
        case Stype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, -0x%llu(%s)", res.mnemonic.c_str(), registerNames[res.rd], res.imm, registerNames[res.rs1]);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
        case Btype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, %s, 0x%llx", res.mnemonic.c_str(), registerNames[res.rs1], registerNames[res.rs2], res.imm);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
        case Utype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, 0x%llx", res.mnemonic.c_str(), registerNames[res.rd], res.imm);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
        case Jtype: {
            char resToken[128];
            int ret = sprintf(reinterpret_cast<char *>(resToken), "%s  %s, 0x%llx", res.mnemonic.c_str(), registerNames[res.rd], res.imm);
            if(ret == -1) return false;
            result.emplace_back(BNInstructionTextTokenType::TextToken, resToken);
            break;
        }
    }
    len = 4;
    return true;
}

bool riscvArch::GetInstructionLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len,
                                         BinaryNinja::LowLevelILFunction &il) {
    return false;
}

BNRegisterInfo riscvArch::GetRegisterInfo(uint32_t reg) {
    switch (reg) {
        case Registers::Zero: return RegisterInfo(Registers::Zero, 0, 8);
        case Registers::Ra: return RegisterInfo(Registers::Ra, 0, 8);
        case Registers::Sp: return RegisterInfo(Registers::Sp, 0, 8);
        case Registers::Gp: return RegisterInfo(Registers::Gp, 0, 8);
        case Registers::Tp: return RegisterInfo(Registers::Tp, 0, 8);
        case Registers::T0: return RegisterInfo(Registers::T0, 0, 8);
        case Registers::T1: return RegisterInfo(Registers::T1, 0, 8);
        case Registers::T2: return RegisterInfo(Registers::T2, 0, 8);
        case Registers::S0: return RegisterInfo(Registers::S0, 0, 8);
        case Registers::S1: return RegisterInfo(Registers::S1, 0, 8);
        case Registers::A0: return RegisterInfo(Registers::A0, 0, 8);
        case Registers::A1: return RegisterInfo(Registers::A1, 0, 8);
        case Registers::A2: return RegisterInfo(Registers::A2, 0, 8);
        case Registers::A3: return RegisterInfo(Registers::A3, 0, 8);
        case Registers::A4: return RegisterInfo(Registers::A4, 0, 8);
        case Registers::A5: return RegisterInfo(Registers::A5, 0, 8);
        case Registers::A6: return RegisterInfo(Registers::A6, 0, 8);
        case Registers::A7: return RegisterInfo(Registers::A7, 0, 8);
        case Registers::S2: return RegisterInfo(Registers::S2, 0, 8);
        case Registers::S3: return RegisterInfo(Registers::S3, 0, 8);
        case Registers::S4: return RegisterInfo(Registers::S4, 0, 8);
        case Registers::S5: return RegisterInfo(Registers::S5, 0, 8);
        case Registers::S6: return RegisterInfo(Registers::S6, 0, 8);
        case Registers::S7: return RegisterInfo(Registers::S7, 0, 8);
        case Registers::S8: return RegisterInfo(Registers::S8, 0, 8);
        case Registers::S9: return RegisterInfo(Registers::S9, 0, 8);
        case Registers::S10: return RegisterInfo(Registers::S10, 0, 8);
        case Registers::S11: return RegisterInfo(Registers::S11, 0, 8);
        case Registers::T3: return RegisterInfo(Registers::T3, 0, 8);
        case Registers::T4: return RegisterInfo(Registers::T4, 0, 8);
        case Registers::T5: return RegisterInfo(Registers::T5, 0, 8);
        case Registers::T6: return RegisterInfo(Registers::T6, 0, 8);
        case Registers::Pc: return RegisterInfo(Registers::Pc, 0, 8);
        default:
            BinaryNinja::Log(ErrorLog, "Unknown RISC-V Register: %d", reg);
            return RegisterInfo(0, 0, 0);
    }
}

BNRegisterInfo riscvArch::RegisterInfo(uint32_t fullWidthReg, size_t offset, size_t size) {
    BNRegisterInfo result{};
    result.fullWidthRegister = fullWidthReg;
    result.offset = offset;
    result.size = size;
    result.extend = NoExtend;
    return result;
}

uint32_t riscvArch::GetStackPointerRegister() {
    return Registers::Pc;
}
