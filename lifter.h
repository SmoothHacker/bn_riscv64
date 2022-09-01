#ifndef BN_RISCV_ARCH_LIFTER_H
#define BN_RISCV_ARCH_LIFTER_H

#include <binaryninjaapi.h>
#include "disassembler.h"

using namespace BinaryNinja;

ExprId store_helper(BinaryNinja::LowLevelILFunction &il, Instruction &inst, uint64_t size);

ExprId load_helper(BinaryNinja::LowLevelILFunction &il, Instruction &inst, uint64_t size, bool isUnsigned);

bool liftToLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, BinaryNinja::LowLevelILFunction &il);

#endif // BN_RISCV_ARCH_LIFTER_H
