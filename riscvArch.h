#ifndef BN_RISCV_ARCH_RISCVARCH_H
#define BN_RISCV_ARCH_RISCVARCH_H

#include <binaryninjaapi.h>

class riscvArch : public BinaryNinja::Architecture {
    size_t addressSize = 32;

    static BNRegisterInfo RegisterInfo(uint32_t fullWidthReg, size_t offset, size_t size);

public:
    explicit riscvArch(const std::string &name) : Architecture(name) {}

    BNEndianness GetEndianness() const override;

    size_t GetAddressSize() const override;

    bool GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen,
                            BinaryNinja::InstructionInfo &result) override;

    bool GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len,
                            std::vector<BinaryNinja::InstructionTextToken> &result) override;

    bool GetInstructionLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len,
                                  BinaryNinja::LowLevelILFunction &il) override;

    BNRegisterInfo GetRegisterInfo(uint32_t reg) override;

    uint32_t GetStackPointerRegister() override;
};

#endif //BN_RISCV_ARCH_RISCVARCH_H