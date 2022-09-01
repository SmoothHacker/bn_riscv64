#include "riscvCallingConvention.h"
#include "lifter.h"

std::vector<uint32_t> riscvCallingConvention::GetCalleeSavedRegisters() {
    std::vector<uint32_t> regs = {sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11};
    return regs;
}

std::vector<uint32_t> riscvCallingConvention::GetCallerSavedRegisters() {
    std::vector<uint32_t> regs = {Zero, t0, t1, t2, a0, a1, a2, a3, a4, a5, a6, a7, t3, t4, t5, t6};
    return regs;
}

std::vector<uint32_t> riscvCallingConvention::GetIntegerArgumentRegisters() {
    std::vector<uint32_t> regs = {a0, a1, a2, a3, a4, a5, a6, a7};
    return regs;
}

bool riscvCallingConvention::AreArgumentRegistersUsedForVarArgs() {
    return true;
}

bool riscvCallingConvention::IsStackReservedForArgumentRegisters() {
    return true;
}

uint32_t riscvCallingConvention::GetIntegerReturnValueRegister() {
    return a0;
}

uint32_t riscvCallingConvention::GetGlobalPointerRegister() {
    return gp;
}
