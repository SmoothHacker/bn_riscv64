#ifndef BN_RISCV_ARCH_DISASSEMBLER_H
#define BN_RISCV_ARCH_DISASSEMBLER_H

#include <cstdint>
#include <cstdlib>
#include <string>

enum Registers {
    Zero = 0,
    Ra,
    Sp,
    Gp,
    Tp,
    T0,
    T1,
    T2,
    S0,
    S1,
    A0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    S10,
    S11,
    T3,
    T4,
    T5,
    T6,
    Pc,
};

static const char *registerNames[] = {
        "Zero",
        "Ra",
        "Sp",
        "Gp",
        "Tp",
        "T0",
        "T1",
        "T2",
        "S0",
        "S1",
        "A0",
        "A1",
        "A2",
        "A3",
        "A4",
        "A5",
        "A6",
        "A7",
        "S2",
        "S3",
        "S4",
        "S5",
        "S6",
        "S7",
        "S8",
        "S9",
        "S10",
        "S11",
        "T3",
        "T4",
        "T5",
        "T6",
        "Pc"};

enum InstrType {
    Rtype,
    Itype,
    Stype,
    Btype,
    Utype,
    Jtype
};

class Instruction {
public:
    InstrType type;
    std::string mnemonic;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint8_t funct3;
    uint64_t imm;
};

class Disassembler {
    static Instruction implRtype(uint32_t instr);

    Instruction implItype(uint32_t instr);

    Instruction implStype(uint32_t instr);

    Instruction implBtype(uint32_t instr);

    Instruction implUtype(uint32_t instr);

    Instruction implJtype(uint32_t instr);

public:
    Instruction disasm(const uint8_t *data, uint64_t addr, size_t maxLen);
};

#endif //BN_RISCV_ARCH_DISASSEMBLER_H
