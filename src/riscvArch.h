#ifndef BN_RISCV_ARCH_RISCVARCH_H
#define BN_RISCV_ARCH_RISCVARCH_H

#include <binaryninjaapi.h>

class riscvArch : public BinaryNinja::Architecture {
    size_t addressSize = 32;
    BNEndianness endian;

    static BNRegisterInfo RegisterInfo(uint32_t fullWidthReg);

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

    std::vector<uint32_t> GetFullWidthRegisters() override;

    std::vector<uint32_t> GetAllRegisters() override;

    std::string GetRegisterStackName(uint32_t regStack) override;

    uint32_t GetLinkRegister() override;
};

#endif //BN_RISCV_ARCH_RISCVARCH_H
