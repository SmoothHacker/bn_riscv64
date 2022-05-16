#include "disassembler.h"

Instruction Disassembler::disasm(const uint8_t *data, uint64_t addr) {
    auto *insdword = (uint32_t *) data;

    uint8_t opcode = *insdword & 0b1111111;
    Instruction instr{
            .type = InstrType::Error,
    };

    switch (opcode) {
        case 0b0110111: {
            // LUI
            instr = implUtype(*insdword);
            instr.mnemonic = InstrName::LUI;
            return instr;
        }
        case 0b0010111: {
            // AUIPC
            instr = implUtype(*insdword);
            instr.mnemonic = InstrName::AUIPC;
            return instr;
        }
        case 0b1101111: {
            // JAL
            instr = implJtype(*insdword);
            instr.mnemonic = InstrName::JAL;

            // Check for pseudo-instruction J
            if (instr.rd == Registers::Zero) {
                instr.imm += addr;
                instr.mnemonic = InstrName::J;
            }
            return instr;
        }
        case 0b1100111: {
            // JALR
            instr = implItype(*insdword);
            if (instr.rd == Registers::Zero) {
                instr.mnemonic = InstrName::RET;
            } else {
                instr.mnemonic = InstrName::JALR;
            }
            return instr;
        }
        case 0b1100011: {
            // Branch Instructions
            instr = implBtype(*insdword);
            instr.imm = instr.imm + addr;

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = InstrName::BEQ;
                    break;
                case 0b001:
                    instr.mnemonic = InstrName::BNE;
                    break;
                case 0b100:
                    instr.mnemonic = InstrName::BLT;
                    break;
                case 0b101:
                    instr.mnemonic = InstrName::BGE;
                    break;
                case 0b110:
                    instr.mnemonic = InstrName::BLTU;
                    break;
                case 0b111:
                    instr.mnemonic = InstrName::BGEU;
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Branch instr", instr.funct3);
                    instr.type = InstrType::Error;
                    instr.mnemonic = InstrName::UNSUPPORTED;
                    break;
            }

            return instr;
        }
        case 0b0000011: {
            // Load Instructions
            instr = implItype(*insdword);

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = InstrName::LB;
                    break;
                case 0b001:
                    instr.mnemonic = InstrName::LH;
                    break;
                case 0b010:
                    instr.mnemonic = InstrName::LW;
                    break;
                case 0b011:
                    instr.mnemonic = InstrName::LD;
                    break;
                case 0b100:
                    instr.mnemonic = InstrName::LBU;
                    break;
                case 0b101:
                    instr.mnemonic = InstrName::LHU;
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Load instr", instr.funct3);
                    instr.type = InstrType::Error;
                    instr.mnemonic = InstrName::UNSUPPORTED;
                    break;
            }

            return instr;
        }
        case 0b0100011: {
            // Store Instructions
            instr = implStype(*insdword);

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = InstrName::SB;
                    break;
                case 0b001:
                    instr.mnemonic = InstrName::SH;
                    break;
                case 0b010:
                    instr.mnemonic = InstrName::SW;
                    break;
                case 0b011:
                    instr.mnemonic = InstrName::SD;
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Store instr", instr.funct3);
                    instr.type = InstrType::Error;
                    instr.mnemonic = InstrName::UNSUPPORTED;
                    break;
            }
            return instr;
        }
        case 0b0010011: {
            // Immediate Arithmetic
            instr = implItype(*insdword);

            switch (instr.funct3) {
                case 0b000: {
                    // Covers pseudo-instr load immediate - li
                    if (instr.rs1 == Registers::Zero) {
                        instr.mnemonic = InstrName::LI;
                    } else {
                        instr.mnemonic = InstrName::ADDI;
                    }
                    break;
                }
                case 0b010:
                    instr.mnemonic = InstrName::SLTI;
                    break;
                case 0b011:
                    instr.mnemonic = InstrName::SLTIU;
                    break;
                case 0b100:
                    instr.mnemonic = InstrName::XORI;
                    break;
                case 0b110:
                    instr.mnemonic = InstrName::ORI;
                    break;
                case 0b111:
                    instr.mnemonic = InstrName::ANDI;
                    break;
                    // TODO implement SLLI, SRLI, SRAI
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Immediate Arithmetic instr",
                                     instr.funct3);
                    instr.type = InstrType::Error;
                    instr.mnemonic = InstrName::UNSUPPORTED;
                    break;
            }
            return instr;
        }
        case 0b0110011: {
            // Register Arithmetic
            instr = implRtype(*insdword);
            switch (instr.funct3) {
                case 0b000: {
                    if (instr.funct7 == 0)
                        instr.mnemonic = InstrName::ADD;
                    else
                        instr.mnemonic = InstrName::SUB;
                    break;
                }
                case 0b001: {
                    instr.mnemonic = InstrName::SLL;
                    break;
                }
                case 0b010: {
                    instr.mnemonic = InstrName::SLT;
                    break;
                }
                case 0b011: {
                    instr.mnemonic = InstrName::SLTU;
                    break;
                }
                case 0b100: {
                    instr.mnemonic = InstrName::XOR;
                    break;
                }
                case 0b101: {
                    if (instr.funct7 == 0)
                        instr.mnemonic = InstrName::SRL;
                    else
                        instr.mnemonic = InstrName::SRA;
                    break;
                }
                case 0b110: {
                    instr.mnemonic = InstrName::OR;
                    break;
                }
                case 0b111: {
                    instr.mnemonic = InstrName::AND;
                    break;
                }
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Register Arithmetic instr", instr.funct3);
                    instr.type = InstrType::Error;
                    instr.mnemonic = InstrName::UNSUPPORTED;
                    break;
            }
            return instr;
        }
        case 0b1110011: {
            // ECALL or EBREAK
            instr = implItype(*insdword);
            if (instr.imm != 0)
                instr.mnemonic = InstrName::EBREAK;
            else
                instr.mnemonic = InstrName::ECALL;
            return instr;
        }
        default:
            BinaryNinja::Log(ErrorLog, "Unimplemented instr - Opcode: 0x%x\n", opcode);
            instr.type = InstrType::Error;
            instr.mnemonic = InstrName::UNSUPPORTED;
            return instr;
    }
}

Instruction Disassembler::implRtype(uint32_t insdword) {
    Instruction instr;
    instr.type = InstrType::Rtype;
    instr.rd = (insdword >> 7) & 0b11111;
    instr.funct3 = (insdword >> 12) & 0b111;
    instr.rs1 = (insdword >> 15) & 0b11111;
    instr.rs2 = (insdword >> 20) & 0b11111;
    instr.funct7 = (insdword >> 25) & 0b11111;

    return instr;
}

Instruction Disassembler::implItype(uint32_t insdword) {
    Instruction instr;
    instr.type = InstrType::Itype;
    instr.rs1 = (insdword >> 15) & 0b11111;
    instr.rd = (insdword >> 7) & 0b11111;
    instr.funct3 = (insdword >> 12) & 0b111;
    instr.imm = ((int32_t) insdword) >> 20;

    return instr;
}

Instruction Disassembler::implStype(uint32_t insdword) {
    uint32_t imm115 = (insdword >> 25) & 0b1111111;
    uint32_t imm40 = (insdword >> 7) & 0b11111;
    uint32_t imm = (imm115 << 5) | imm40;

    Instruction instr;
    instr.type = Stype;
    instr.imm = (((int32_t) imm) << 20) >> 20;
    instr.rs2 = (insdword >> 20) & 0b11111;
    instr.rs1 = (insdword >> 15) & 0b11111;
    instr.funct3 = (insdword >> 12) & 0b111;

    return instr;
}

Instruction Disassembler::implBtype(uint32_t insdword) {
    uint32_t imm12 = (insdword >> 31) & 1;
    uint32_t imm105 = (insdword >> 25) & 0b111111;
    uint32_t imm41 = (insdword >> 8) & 0b1111;
    uint32_t imm11 = (insdword >> 7) & 1;
    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm105 << 5) | (imm41 << 1);

    Instruction instr;

    instr.type = Btype;
    instr.imm = ((int32_t) imm << 19) >> 19;
    instr.rs2 = (insdword >> 20) & 0b11111;
    instr.rs1 = (insdword >> 15) & 0b11111;
    instr.funct3 = (insdword >> 12) & 0b111;
    return instr;
}

Instruction Disassembler::implUtype(uint32_t insdword) {
    Instruction instr;
    instr.type = Utype;
    instr.imm = (insdword & !0xfff);
    instr.rd = (insdword >> 7) & 0b11111;

    return instr;
}

Instruction Disassembler::implJtype(uint32_t insdword) {
    uint32_t imm20 = (insdword >> 31) & 1;
    uint32_t imm101 = (insdword >> 21) & 0b1111111111;
    uint32_t imm11 = (insdword >> 20) & 1;
    uint32_t imm1912 = (insdword >> 12) & 0b11111111;
    uint32_t imm = (imm20 << 20) | (imm1912 << 12) | (imm11 << 11) | (imm101 << 1);

    Instruction instr;
    instr.type = Jtype;
    instr.imm = ((int64_t) imm << 11) >> 11;
    instr.rd = (insdword >> 7) & 0b11111;
    return instr;
}
