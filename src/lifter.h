#ifndef BN_RISCV_ARCH_LIFTER_H
#define BN_RISCV_ARCH_LIFTER_H

#include "disassembler.h"
#include <binaryninjaapi.h>

using namespace BinaryNinja;

ExprId store_helper(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	uint64_t size);

ExprId load_helper(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	uint64_t size, bool isUnsigned);

void liftToLowLevelIL(Architecture* arch, const uint8_t* data, uint64_t addr, size_t& len,
	BinaryNinja::LowLevelILFunction& il);

#endif // BN_RISCV_ARCH_LIFTER_H
