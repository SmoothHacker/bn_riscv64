#ifndef BN_RISCV_ARCH_RISCVCALLINGCONVENTION_H
#define BN_RISCV_ARCH_RISCVCALLINGCONVENTION_H

#include <binaryninjaapi.h>

using namespace BinaryNinja;

class riscvCallingConvention : public CallingConvention {
public:
    explicit riscvCallingConvention(Architecture *arch) : CallingConvention(arch, "RISC-V") {};

    std::vector<uint32_t> GetCallerSavedRegisters() override;

    std::vector<uint32_t> GetCalleeSavedRegisters() override;

    std::vector<uint32_t> GetIntegerArgumentRegisters() override;

    bool AreArgumentRegistersUsedForVarArgs() override;

    bool IsStackReservedForArgumentRegisters() override;

    uint32_t GetIntegerReturnValueRegister() override;

    uint32_t GetGlobalPointerRegister() override;
};

#endif // BN_RISCV_ARCH_RISCVCALLINGCONVENTION_H
