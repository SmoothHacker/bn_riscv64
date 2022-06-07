#ifndef BN_RISCV_ARCH_LIFTER_H
#define BN_RISCV_ARCH_LIFTER_H

#include <binaryninjaapi.h>
#include "disassembler.h"

using namespace BinaryNinja;

bool liftToLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, BinaryNinja::LowLevelILFunction &il);

#endif // BN_RISCV_ARCH_LIFTER_H
