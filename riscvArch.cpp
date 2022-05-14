#include "riscvArch.h"

BNEndianness riscvArch::GetEndianness() const {
    return BigEndian;
}

size_t riscvArch::GetAddressSize() const {
    return 0;
}

bool
riscvArch::GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen, BinaryNinja::InstructionInfo &result) {
    return false;
}

bool riscvArch::GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len,
                                   std::vector<BinaryNinja::InstructionTextToken> &result) {
    return false;
}
