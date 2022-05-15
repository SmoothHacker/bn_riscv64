#ifndef BN_RISCV_ARCH_RISCVARCH_H
#define BN_RISCV_ARCH_RISCVARCH_H

#include <binaryninjaapi.h>

class riscvArch : public BinaryNinja::Architecture {
    size_t addressSize = 32;
    BNEndianness endian;

    static BNRegisterInfo RegisterInfo(uint32_t fullWidthReg, size_t size);

public:
    riscvArch(const std::string &name, BNEndianness endian_);

    BNEndianness GetEndianness() const override;

    size_t GetAddressSize() const override;

    size_t GetDefaultIntegerSize() const override;

    size_t GetMaxInstructionLength() const override;

    std::string GetRegisterName(uint32_t reg) override;

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
