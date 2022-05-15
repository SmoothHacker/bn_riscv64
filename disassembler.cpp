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
            instr.mnemonic = std::string("lui");
            return instr;
        }
        case 0b0010111: {
            // AUIPC
            instr = implUtype(*insdword);
            instr.mnemonic = std::string("auipc");
            return instr;
        }
        case 0b1101111: {
            // JAL
            instr = implJtype(*insdword);
            instr.mnemonic = std::string("jal");
            return instr;
        }
        case 0b1100111: {
            // JALR
            instr = implItype(*insdword);
            instr.mnemonic = std::string("jalr");
            return instr;
        }
        case 0b1100011: {
            // Branch Instructions
            instr = implBtype(*insdword);

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = std::string("beq");
                    break;
                case 0b001:
                    instr.mnemonic = std::string("bne");
                    break;
                case 0b100:
                    instr.mnemonic = std::string("blt");
                    break;
                case 0b101:
                    instr.mnemonic = std::string("bge");
                    break;
                case 0b110:
                    instr.mnemonic = std::string("bltu");
                    break;
                case 0b111:
                    instr.mnemonic = std::string("bgeu");
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Branch instr", instr.funct3);
                    instr.type = InstrType::Error;
                    break;
            }

            return instr;
        }
        case 0b0000011: {
            // Load Instructions
            instr = implItype(*insdword);

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = std::string("lb");
                    break;
                case 0b001:
                    instr.mnemonic = std::string("lh");
                    break;
                case 0b010:
                    instr.mnemonic = std::string("lw");
                    break;
                case 0b100:
                    instr.mnemonic = std::string("lbu");
                    break;
                case 0b101:
                    instr.mnemonic = std::string("lhu");
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Load instr", instr.funct3);
                    instr.type = InstrType::Error;
                    break;
            }

            return instr;
        }
        case 0b0100011: {
            // Store Instructions
            instr = implStype(*insdword);

            switch (instr.funct3) {
                case 0b000:
                    instr.mnemonic = std::string("sb");
                    break;
                case 0b001:
                    instr.mnemonic = std::string("sh");
                    break;
                case 0b010:
                    instr.mnemonic = std::string("sw");
                    break;
                case 0b011:
                    instr.mnemonic = std::string("sd");
                    break;
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Store instr", instr.funct3);
                    instr.type = InstrType::Error;
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
                    instr.mnemonic = std::string("addi");
                    break;
                }
                case 0b010:
                    instr.mnemonic = std::string("slti");
                    break;
                case 0b011:
                    instr.mnemonic = std::string("sltiu");
                    break;
                case 0b100:
                    instr.mnemonic = std::string("xori");
                    break;
                case 0b110:
                    instr.mnemonic = std::string("ori");
                    break;
                case 0b111:
                    instr.mnemonic = std::string("andi");
                    break;
                    // TODO implement SLLI, SRLI, SRAI
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Immediate Arithmetic instr",
                                     instr.funct3);
                    instr.type = InstrType::Error;
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
                        instr.mnemonic = std::string("add");
                    else
                        instr.mnemonic = std::string("sub");
                    break;
                }
                case 0b001: {
                    instr.mnemonic = std::string("sll");
                    break;
                }
                case 0b010: {
                    instr.mnemonic = std::string("slt");
                    break;
                }
                case 0b011: {
                    instr.mnemonic = std::string("sltu");
                    break;
                }
                case 0b100: {
                    instr.mnemonic = std::string("xor");
                    break;
                }
                case 0b101: {
                    if (instr.funct7 == 0)
                        instr.mnemonic = std::string("srl");
                    else
                        instr.mnemonic = std::string("sra");
                    break;
                }
                case 0b110: {
                    instr.mnemonic = std::string("or");
                    break;
                }
                case 0b111: {
                    instr.mnemonic = std::string("and");
                    break;
                }
                default:
                    BinaryNinja::Log(ErrorLog, "Unknown funct3 [%d] for Register Arithmetic instr", instr.funct3);
                    instr.type = InstrType::Error;
                    break;
            }
            return instr;
        }
        case 0b1110011: {
            // ECALL or EBREAK
            instr = implItype(*insdword);
            if (instr.imm != 0)
                instr.mnemonic = std::string("EBREAK");
            else
                instr.mnemonic = std::string("ECALL");
            return instr;
        }
        default:
            BinaryNinja::Log(ErrorLog, "Unimplemented instr - Opcode: 0x%x\n", opcode);
            instr.type = InstrType::Error;
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
