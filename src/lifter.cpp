#include "lifter.h"

ExprId cond_branch(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	ExprId condition) {
	uint64_t dest = inst.imm + il.GetCurrentAddress();
	uint64_t nextInst = il.GetCurrentAddress() + 4;
	auto trueLabel = il.GetLabelForAddress(il.GetArchitecture(), dest);
	auto falseLabel = il.GetLabelForAddress(il.GetArchitecture(), nextInst);

	if (trueLabel && falseLabel)
		return il.If(condition, *trueLabel, *falseLabel);

	LowLevelILLabel trueCode, falseCode;
	if (trueLabel) {
		il.AddInstruction(il.If(condition, *trueLabel, falseCode));
		il.MarkLabel(trueCode);
		return il.Jump(il.ConstPointer(8, nextInst));
	}

	if (falseLabel) {
		il.AddInstruction(il.If(condition, trueCode, *falseLabel));
		il.MarkLabel(falseCode);
		return il.Jump(il.ConstPointer(8, dest));
	}

	il.AddInstruction(il.If(condition, trueCode, falseCode));
	il.MarkLabel(trueCode);
	il.AddInstruction(il.Jump(il.ConstPointer(8, dest)));
	il.MarkLabel(falseCode);
	return il.Jump(il.ConstPointer(8, nextInst));
}

ExprId store_helper(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	uint64_t size) {
	ExprId addr = il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm));
	ExprId val = il.Register(size, inst.rs2);

	if (inst.mnemonic == SD)
		return il.Store(size, addr, il.LowPart(size, val));
	else
		return il.Store(size, addr, val);
};

ExprId load_helper(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	uint64_t size, bool isUnsigned) {
	ExprId addr = il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm));
	if (isUnsigned)
		return il.SetRegister(8, inst.rd, il.ZeroExtend(8, il.Load(size, addr)));
	else
		return il.SetRegister(8, inst.rd, il.SignExtend(8, il.Load(size, addr)));
}

void liftToLowLevelIL(const uint8_t* data, uint64_t addr, size_t& len,
	BinaryNinja::LowLevelILFunction& il) {
	Instruction inst = Disassembler::disasm(data, addr);
	ExprId expr = il.Unimplemented();
	switch (inst.mnemonic) {
	case ADDI:
		expr = il.SetRegister(
			8, inst.rd, il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm)));
		break;
	case ADD:
		expr = il.SetRegister(
			8, inst.rd,
			il.Add(4, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SUB:
		expr = il.SetRegister(
			8, inst.rd,
			il.Sub(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case AUIPC:
		expr = il.SetRegister(8, inst.rd, il.Const(8, inst.imm + addr));
		break;
	case JAL:
		break;
	case JALR: {
		ExprId rs1;
		if (inst.rs1 != 0)
			rs1 = il.Register(8, inst.rs1);
		else
			rs1 = il.Const(8, 0);

		ExprId target = il.Add(8, rs1, il.Const(8, inst.imm));
		if (inst.rd != 0) // Link
		                  // il.AddInstruction(il.SetRegister(8, inst.rd,
		                  // il.Add(8, rs1, il.Const(8, -inst.imm + addr))));

			expr = il.Call(target);
	} break;
	case BEQ:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(
				il, inst,
				il.CompareEqual(8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else
			expr = cond_branch(il, inst,
				il.CompareEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BNE:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(
				il, inst,
				il.CompareNotEqual(8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else
			expr = cond_branch(il, inst,
				il.CompareNotEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BLT:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(il, inst,
				il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
					il.Const(8, 0)));
		else if (inst.rs1 == Registers::Zero)
			expr = cond_branch(il, inst,
				il.CompareSignedLessThan(8, il.Const(8, 0),
					il.Register(8, inst.rs2)));
		else
			expr = cond_branch(il, inst,
				il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BGE:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(il, inst,
				il.CompareSignedGreaterEqual(
					8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else if (inst.rs1 == Registers::Zero)
			expr = cond_branch(il, inst,
				il.CompareSignedGreaterEqual(
					8, il.Const(8, 0), il.Register(8, inst.rs2)));
		else
			expr = cond_branch(il, inst,
				il.CompareSignedGreaterEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BLTU:
		expr = cond_branch(il, inst,
			il.CompareUnsignedLessThan(8, il.Register(8, inst.rs1),
				il.Register(8, inst.rs2)));
		break;
	case BGEU:
		expr = cond_branch(il, inst,
			il.CompareUnsignedGreaterEqual(8, il.Register(8, inst.rs1),
				il.Register(8, inst.rs2)));
		break;
	case LB:
		expr = load_helper(il, inst, 1, false);
		break;
	case LH:
		expr = load_helper(il, inst, 2, false);
		break;
	case LBU:
		expr = load_helper(il, inst, 1, true);
		break;
	case LHU:
		expr = load_helper(il, inst, 2, true);
		break;
	case LWU:
		expr = load_helper(il, inst, 4, true);
		break;
	case LW:
		expr = load_helper(il, inst, 4, false);
		break;
	case LD:
		expr = load_helper(il, inst, 8, false);
		break;
	case SB:
		expr = store_helper(il, inst, 1);
		break;
	case SH:
		expr = store_helper(il, inst, 2);
		break;
	case SW:
		expr = store_helper(il, inst, 4);
		break;
	case SD:
		expr = store_helper(il, inst, 8);
		break;
	case SLTI:
		expr = il.SetRegister(8, inst.rd,
			il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
				il.Const(8, inst.imm)));
		break;
	case SLTIU:
		expr = il.SetRegister(8, inst.rd,
			il.CompareUnsignedLessThan(8, il.Register(8, inst.rs1),
				il.Const(8, inst.imm)));
		break;
	case XORI:
		expr = il.SetRegister(4, inst.rd,
			il.Xor(4, il.Register(4, inst.rs1),
				il.SignExtend(4, il.Const(3, inst.imm))));
		break;
	case ORI:
		expr = il.SetRegister(4, inst.rd,
			il.Or(4, il.Register(4, inst.rs1),
				il.SignExtend(4, il.Const(3, inst.imm))));
		break;
	case ANDI:
		expr = il.SetRegister(
			8, inst.rd,
			il.And(8, il.Register(8, inst.rs1), il.Const(12, inst.imm)));
		break;
	case SLLI:
		expr = il.SetRegister(
			8, inst.rd,
			il.ShiftLeft(8, il.Register(8, inst.rs1), il.Const(8, inst.funct3)));
		break;
	case SRLI:
		expr = il.SetRegister(8, inst.rd,
			il.ArithShiftRight(8, il.Register(4, inst.rs1),
				il.Const(8, inst.funct3)));
		break;
	case SLL:
		break;
	case SLT:
		expr = il.SetRegister(8, inst.rd,
			il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
				il.Const(8, inst.rs2)));
		break;
	case SLTU:
		expr = il.SetRegister(8, inst.rd,
			il.CompareUnsignedLessThan(8, il.Register(8, inst.rs1),
				il.Const(8, inst.rs2)));
		break;
	case XOR:
		expr = il.SetRegister(
			8, inst.rd,
			il.Xor(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SRL:
		break;
	case SRA:
		break;
	case OR:
		expr = il.SetRegister(
			8, inst.rd,
			il.Or(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case AND:
		expr = il.SetRegister(
			8, inst.rd,
			il.And(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case FENCE:
		break;
	case ECALL:
		expr = il.SystemCall();
		break;
	case EBREAK:
		expr = il.Breakpoint();
		break;
	case SRAI:
		expr = il.SetRegister(8, inst.rd,
			il.ArithShiftRight(8, il.Register(8, inst.rs1),
				il.Const(8, inst.funct3)));
		break;
	case ADDIW:
		expr = il.SetRegister(
			8, inst.rd, il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm)));
		break;
	case SLLIW:
		expr = il.SetRegister(
			4, inst.rd,
			il.ShiftLeft(4, il.Register(4, inst.rs1), il.Const(4, inst.funct3)));
		break;
	case SRLIW:
		expr = il.SetRegister(4, inst.rd,
			il.LogicalShiftRight(4, il.Register(4, inst.rs1),
				il.Const(4, inst.funct3)));
		break;
	case SRAIW:
		break;
	case ADDW:
		break;
	case SUBW:
		break;
	case SLLW:
		break;
	case SRLW:
		break;
	case SRAW:
		break;
	case J:
		expr = il.Jump(il.Const(8, inst.imm));
		break;
	case LI:
		expr = il.SetRegister(8, inst.rd, il.ConstPointer(8, inst.imm));
		break;
	case LUI:
		expr = il.SetRegister(8, inst.rd,
			il.ShiftLeft(8, il.ZeroExtend(8, il.Const(3, inst.imm)),
				il.Const(8, 12)));
		break;
	case RET:
		expr = il.Return(il.Register(8, inst.rs1));
		break;
	default:
		break;
	}
	il.AddInstruction(expr);
}