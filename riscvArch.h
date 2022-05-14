#ifndef BN_RISCV_ARCH_RISCVARCH_H
#define BN_RISCV_ARCH_RISCVARCH_H

#include <binaryninjaapi.h>

class riscvArch: public BinaryNinja::Architecture {
private:
    BNEndianness endianness;

public:
    BNEndianness GetEndianness() const override;

    size_t GetAddressSize() const override;

    bool GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen,
                            BinaryNinja::InstructionInfo &result) override;

    bool GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len,
                            std::vector<BinaryNinja::InstructionTextToken> &result) override;
};

#endif //BN_RISCV_ARCH_RISCVARCH_H
