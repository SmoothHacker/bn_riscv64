#ifndef BN_RISCV_ARCH_DISASSEMBLER_H
#define BN_RISCV_ARCH_DISASSEMBLER_H

#include <cstdint>
#include <cstdlib>
#include <string>

#include <binaryninjaapi.h>

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
        "zero",
        "ra",
        "sp",
        "gp",
        "tp",
        "t0",
        "t1",
        "t2",
        "s0",
        "s1",
        "a0",
        "a1",
        "a2",
        "a3",
        "a4",
        "a5",
        "a6",
        "a7",
        "s2",
        "s3",
        "s4",
        "s5",
        "s6",
        "s7",
        "s8",
        "s9",
        "s10",
        "s11",
        "t3",
        "t4",
        "t5",
        "t6",
        "pc"
};

enum InstrType {
    Error = 0,
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
    size_t rd;
    size_t rs1;
    size_t rs2;
    uint32_t funct7;
    uint32_t funct3;
    int64_t imm;
};

class Disassembler {
    static Instruction implRtype(uint32_t instr);

    static Instruction implItype(uint32_t instr);

    static Instruction implStype(uint32_t instr);

    static Instruction implBtype(uint32_t instr);

    static Instruction implUtype(uint32_t instr);

    static Instruction implJtype(uint32_t instr);

public:
    static Instruction disasm(const uint8_t *data, uint64_t addr, size_t maxLen);
};

#endif //BN_RISCV_ARCH_DISASSEMBLER_H
