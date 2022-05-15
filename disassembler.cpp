#include "disassembler.h"

Instruction Disassembler::disasm(const uint8_t *data, uint64_t addr, size_t maxLen) {
    uint32_t *inst = (uint32_t *) data;
    uint8_t opcode = *data & 0b1111111;

    switch (opcode) {
        case 0b0110111: {
            // LUI
            Instruction instruction = implUtype(*inst);
            return instruction;
        }
        case 0b0010111: {
            // AUIPC
            Instruction instruction = implUtype(*inst);
            return instruction;
        }
        case 0b1101111: {
            // JAL
            Instruction instruction = implJtype(*inst);
            return instruction;
        }
        case 0b1100111: {
            // JALR
            Instruction instruction = implItype(*inst);
            return instruction;
        }
        case 0b1100011: {
            // Branch Instructions
            Instruction instruction = implBtype(*inst);

            switch (instruction.funct3) {
                case 0b000: {
                    instruction.mnemonic = std::string("BEQ");
                }
                case 0b001: {
                    instruction.mnemonic = std::string("BNE");
                }
                case 0b100: {
                }
                case 0b101: {
                }
                case 0b110: {
                }
                case 0b111: {
                }
                default:
                    printf("Unknown funct3 for Branch instr");
            }

            return instruction;
        }
        case 0b0000011: {
            // Load Instructions
            Instruction instruction = implItype(*inst);

            switch (instruction.funct3) {
                case 0b000: {
                }
                case 0b001: {
                }
                case 0b010: {
                }
                case 0b100: {
                }
                case 0b101: {
                }
                default:
                    instruction.mnemonic = std::string("");
                    break;
            }

            return instruction;
        }
        case 0b0100011: {
            // Store Instructions
            Instruction instruction = implStype(*inst);

            switch (instruction.funct3) {
                case 0b000: {
                }
                case 0b001: {
                }
                case 0b010: {
                    instruction.mnemonic = std::string("sw");
                }
                case 0b011: {
                    instruction.mnemonic = std::string("sd");
                }
                default:
                    break;
            }

            return instruction;
        }
        case 0b0010011: {
            // Immediate Arithmetic
            Instruction instruction = implItype(*inst);

            switch (instruction.funct3) {
                case 0b000: {
                    // ADDI
                    instruction.mnemonic = std::string("addi");
                }
                default:
                    break;
            }
            return instruction;
        }
        case 0b0110011: {
            // Register Arithmetic
            Instruction instruction = implRtype(*inst);
            instruction.mnemonic = std::string("");
            return instruction;
        }
        case 0b1110011: {
            // ECALL or EBREAK
            Instruction instruction = implItype(*inst);
            if (instruction.imm == 1)
                instruction.mnemonic = std::string("EBREAK");
            else
                instruction.mnemonic = std::string("ECALL");
            return instruction;
        }
        default:
            printf("Unimplemented instruction - Opcode: 0x%x", opcode);
    }

    return {};
}

Instruction Disassembler::implRtype(uint32_t instr) {
    Instruction instrStruct;
    instrStruct.type = InstrType::Rtype;
    instrStruct.rd = (instr >> 7) & 0b11111;
    instrStruct.funct3 = (instr >> 12) & 0b111;
    instrStruct.rs1 = (instr >> 15) & 0b11111;
    instrStruct.rs2 = (instr >> 20) & 0b11111;
    instrStruct.funct7 = (instr >> 25) & 0b11111;

    return instrStruct;
}

Instruction Disassembler::implItype(uint32_t instr) {
    Instruction instrStruct;
    instrStruct.type = InstrType::Itype;
    instrStruct.rs1 = (instr >> 15) & 0b11111;
    instrStruct.rd = (instr >> 7) & 0b11111;
    instrStruct.funct3 = (instr >> 12) & 0b111;
    instrStruct.imm = ((int32_t) instr) >> 20;

    return instrStruct;
}

Instruction Disassembler::implStype(uint32_t instr) {
    uint32_t imm115 = (instr >> 25) & 0b1111111;
    uint32_t imm40 = (instr >> 7) & 0b11111;
    uint32_t imm = (imm115 << 5) | imm40;

    Instruction instruction;
    instruction.type = Stype;
    instruction.imm = ((int32_t) imm << 20) >> 20;
    instruction.rs2 = (instr >> 20) & 0b11111;
    instruction.rs1 = (instr >> 15) & 0b11111;
    instruction.funct3 = (instr >> 12) & 0b111;

    return instruction;
}

Instruction Disassembler::implBtype(uint32_t instr) {
    uint32_t imm12 = (instr >> 31) & 1;
    uint32_t imm105 = (instr >> 25) & 0b111111;
    uint32_t imm41 = (instr >> 8) & 0b1111;
    uint32_t imm11 = (instr >> 7) & 1;
    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm105 << 5) | (imm41 << 1);

    Instruction instruction;

    instruction.type = Btype;
    instruction.imm = ((int32_t) imm << 19) >> 19;
    instruction.rs2 = (instr >> 20) & 0b11111;
    instruction.rs1 = (instr >> 15) & 0b11111;
    instruction.funct3 = (instr >> 12) & 0b111;
    return instruction;
}

Instruction Disassembler::implUtype(uint32_t instr) {
    Instruction instruction;
    instruction.type = Utype;
    instruction.imm = (instr & !0xfff);
    instruction.rd = (instr >> 7) & 0b11111;

    return instruction;
}

Instruction Disassembler::implJtype(uint32_t instr) {
    uint32_t imm20 = (instr >> 31) & 1;
    uint32_t imm101 = (instr >> 21) & 0b1111111111;
    uint32_t imm11 = (instr >> 20) & 1;
    uint32_t imm1912 = (instr >> 12) & 0b11111111;
    uint32_t imm = (imm20 << 20) | (imm1912 << 12) | (imm11 << 11) | (imm101 << 1);

    Instruction instruction;
    instruction.type = Jtype;
    instruction.imm = ((int32_t) imm << 11) >> 11;
    instruction.rd = (instr >> 7) & 0b11111;
    return instruction;
}
