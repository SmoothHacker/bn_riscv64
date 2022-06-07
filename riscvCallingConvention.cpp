#include "riscvCallingConvention.h"

std::vector<uint32_t> riscvCallingConvention::GetCalleeSavedRegisters() {
    std::vector<uint32_t> regs = {2, 8, 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
    return regs;
}

std::vector<uint32_t> riscvCallingConvention::GetCallerSavedRegisters() {
    std::vector<uint32_t> regs = {1, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17, 28, 29, 30, 31};
    return regs;
}

std::vector<uint32_t> riscvCallingConvention::GetIntegerArgumentRegisters() {
    std::vector<uint32_t> regs = {10, 11, 12, 13, 14, 15, 16, 17};
    return regs;
}

bool riscvCallingConvention::AreArgumentRegistersUsedForVarArgs() {
    return true;
}

bool riscvCallingConvention::IsStackReservedForArgumentRegisters() {
    return true;
}

uint32_t riscvCallingConvention::GetIntegerReturnValueRegister() {
    return 1;
}

uint32_t riscvCallingConvention::GetGlobalPointerRegister() {
    return 3;
}
