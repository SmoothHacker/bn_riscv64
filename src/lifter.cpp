#include "lifter.h"
#include "binaryninjaapi.h"
#include "disassembler.h"

ExprId cond_branch(Architecture *arch, BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	ExprId condition) {
	uint64_t dest = inst.imm + il.GetCurrentAddress();
	uint64_t nextInst = il.GetCurrentAddress() + 4;

	BNLowLevelILLabel* trueLabel = il.GetLabelForAddress(arch, dest);
	BNLowLevelILLabel* falseLabel = il.GetLabelForAddress(arch, nextInst);

	if (trueLabel && falseLabel)
		return il.If(condition, *trueLabel, *falseLabel);

	LowLevelILLabel trueCode, falseCode;
	if (trueLabel) {
		il.AddInstruction(il.If(condition, *trueLabel, falseCode));
		il.MarkLabel(falseCode);
		return il.Jump(il.ConstPointer(8, nextInst));
	}

	if (falseLabel) {
		il.AddInstruction(il.If(condition, trueCode, *falseLabel));
		il.MarkLabel(trueCode);
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
	if (inst.rs2 == Registers::Zero) {
		return il.Nop();
	}
	ExprId addr = il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm));
	ExprId val = il.Register(8, inst.rs2);
	return il.Store(size, addr, val);
};

ExprId load_helper(BinaryNinja::LowLevelILFunction& il, Instruction& inst,
	uint64_t size, bool shouldZeroExtend) {
	if (inst.rd == Registers::Zero) {
		return il.Nop();
	}
	ExprId addr = il.Add(8, il.Register(8, inst.rs1), il.Const(8, inst.imm));
	if (inst.mnemonic == InstrName::LW)
		return il.SetRegister(8, inst.rd, il.Load(size, addr));
	else if (shouldZeroExtend)
		return il.SetRegister(8, inst.rd, il.ZeroExtend(8, il.Load(size, addr)));
	else
		return il.SetRegister(8, inst.rd, il.SignExtend(8, il.Load(size, addr)));
}

void liftToLowLevelIL(Architecture *arch, const uint8_t* data, uint64_t addr, size_t& len,
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
			il.Add(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SUB:
		expr = il.SetRegister(
			8, inst.rd,
			il.Sub(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case AUIPC:
		expr = il.SetRegister(8, inst.rd, il.Const(4, (inst.imm << 12) + addr));
		break;
	case JAL: {
		// link
		il.AddInstruction(il.SetRegister(8, inst.rd, il.Const(8, addr + 4)));

		// Jump
		ExprId target = il.Add(8, il.Const(8, addr), il.Const(8, inst.imm));
		expr = il.Jump(target);
	} break;
	case JALR: {
		// JALR has to follow a set of return-address-stack (RAS) actions
		/*if((inst.rd == Registers::ra || inst.rd == Registers::t0) && (inst.rs1 == Registers::ra || inst.rs1 == Registers::t0)) {
			// Check if rs1 == rd
			if (inst.rs1 != inst.rd) {
				// pop, then push
				il.AddInstruction(il.Pop(8, ));
			}
			// push
			il.AddInstruction(il.Push(8, target));
		} else if((inst.rd == Registers::ra || inst.rd == Registers::t0) && (inst.rs1 != Registers::ra && inst.rs1 != Registers::t0)) {
			// push
		} else if((inst.rd != Registers::ra && inst.rd != Registers::t0) && (inst.rs1 == Registers::ra || inst.rs1 == Registers::t0)) {
			// pop
		}*/

		ExprId target = il.Add(8, il.Const(8, inst.imm), il.Register(8, inst.rs1));
		expr = il.Call(target);
/*
		ExprId rs1;
		if (inst.rs1 != Registers::Zero)
			rs1 = il.Register(8, inst.rs1);
		else
			rs1 = il.Const(8, 0);
		ExprId target = il.Add(8, rs1, il.Const(8, inst.imm));

		if (inst.rd != Registers::Zero) {
			il.AddInstruction(il.SetRegister(8, inst.rd, il.Add(8, rs1, il.Const(8, inst.imm + addr))));
		}
		expr = il.Jump(target);*/
	} break;
	case BEQ:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(
				arch,
				il, inst,
				il.CompareEqual(8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else
			expr = cond_branch(arch, il, inst,
				il.CompareEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BNE:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(
				arch,
				il, inst,
				il.CompareNotEqual(8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else
			expr = cond_branch(
				arch, il, inst,
				il.CompareNotEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BLT:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(arch, il, inst,
				il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
					il.Const(8, 0)));
		else if (inst.rs1 == Registers::Zero)
			expr = cond_branch(arch, il, inst,
				il.CompareSignedLessThan(8, il.Const(8, 0),
					il.Register(8, inst.rs2)));
		else
			expr = cond_branch(arch, il, inst,
				il.CompareSignedLessThan(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BGE:
		if (inst.rs2 == Registers::Zero)
			expr = cond_branch(arch, il, inst,
				il.CompareSignedGreaterEqual(
					8, il.Register(8, inst.rs1), il.Const(8, 0)));
		else if (inst.rs1 == Registers::Zero)
			expr = cond_branch(arch, il, inst,
				il.CompareSignedGreaterEqual(
					8, il.Const(8, 0), il.Register(8, inst.rs2)));
		else
			expr = cond_branch(arch, il, inst,
				il.CompareSignedGreaterEqual(8, il.Register(8, inst.rs1),
					il.Register(8, inst.rs2)));
		break;
	case BLTU:
		expr = cond_branch(arch, il, inst,
			il.CompareUnsignedLessThan(8, il.Register(8, inst.rs1),
				il.Register(8, inst.rs2)));
		break;
	case BGEU:
		expr = cond_branch(arch, il, inst,
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
		expr = load_helper(il, inst, 8, true);
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
		expr = il.SetRegister(8, inst.rd,
			il.ShiftLeft(8, il.Register(8, inst.rs1), il.Const(8, inst.imm)));
		break;
	case SRLI:
		expr = il.SetRegister(8, inst.rd,
			il.ArithShiftRight(8, il.Register(8, inst.rs1),
				il.Const(8, inst.imm)));
		break;
	case SLL:
		expr = il.SetRegister(8, inst.rd, il.ShiftLeft(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SLT: {
		ExprId operand;
		if (inst.rs1 == Registers::Zero)
			operand = il.Const(8, 0);
		else
			operand = il.Register(8, inst.rs1);

		expr = il.SetRegister(8, inst.rd,
			il.CompareSignedLessThan(8, operand,
				il.Register(8, inst.rs2)));
		break;
	}
		break;
	case SLTU: {
		ExprId operand;
		if (inst.rs1 == Registers::Zero)
			operand = il.Const(8, 0);
		else
			operand = il.Register(8, inst.rs1);

		expr = il.SetRegister(8, inst.rd,
			il.CompareUnsignedLessThan(8, operand,
				il.Register(8, inst.rs2)));
		break;
	}
	case XOR:
		expr = il.SetRegister(
			8, inst.rd,
			il.Xor(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SRL:
		expr = il.SetRegister(8, inst.rd, il.LogicalShiftRight(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
		break;
	case SRA:
		expr = il.SetRegister(8, inst.rd, il.ArithShiftRight(8, il.Register(8, inst.rs1), il.Register(8, inst.rs2)));
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
		expr = il.Nop();
		break;
	case ECALL:
		expr = il.SystemCall();
		break;
	case EBREAK:
		expr = il.Breakpoint();
		break;
	case SRAI:
		expr = il.SetRegister(8, inst.rd,
			il.ArithShiftRight(8, il.Register(8, inst.rs1), il.Const(8, inst.imm & 0xf)));
		break;
	case ADDIW:
		expr = il.SetRegister(
			8, inst.rd, il.Add(4, il.Register(4, inst.rs1), il.Const(4, inst.imm)));
		break;
	case SLLIW:
		expr = il.SetRegister(
			4, inst.rd,
			il.ShiftLeft(4, il.Register(4, inst.rs1), il.Const(4, inst.rs2)));
		break;
	case SRLIW:
		expr = il.SetRegister(4, inst.rd,
			il.LogicalShiftRight(4, il.Register(4, inst.rs1), il.Const(4, inst.rs2)));
		break;
	case SRAIW:
		expr = il.SetRegister(4, inst.rd,
			il.ArithShiftRight(4, il.Register(4, inst.rs1), il.Const(4, inst.rs2)));
		break;
	case ADDW:
		expr = il.SetRegister(4, inst.rd,
				il.Add(4, il.Register(4, inst.rs1), il.Register(4, inst.rs2)));
		break;
	case SUBW:
		expr = il.SetRegister(4, inst.rd,
			il.Sub(4, il.Register(4, inst.rs1), il.Register(4, inst.rs2)));
		break;
	case SLLW:
		expr = il.SetRegister(4, inst.rd, il.ShiftLeft(4, il.Register(4, inst.rs1), il.Register(4, inst.rs2)));
		break;
	case SRLW:
		expr = il.SetRegister(4, inst.rd, il.LogicalShiftRight(4, il.Register(4, inst.rs1), il.Register(4, inst.rs2)));
		break;
	case SRAW:
		expr = il.SetRegister(4, inst.rd, il.ArithShiftRight(4, il.Register(4, inst.rs1), il.Register(4, inst.rs2)));
		break;
	case J:
		expr = il.Jump(il.Const(8, inst.imm));
		break;
	case LI:
		expr = il.SetRegister(8, inst.rd, il.ConstPointer(8, inst.imm));
		break;
	case LUI:
		expr = il.SetRegister(8, inst.rd, il.Const(4, (int64_t)(inst.imm << 12)));
		break;
	case RET:
		expr = il.Return(il.Register(8, inst.rs1));
		break;
	case MV:
		expr = il.SetRegister(8, inst.rd, il.Register(8, inst.rs1));
		break;
	case JR:
		expr = il.Jump(il.Register(8, inst.rs1));
		break;
	default:
		break;
	}
	il.AddInstruction(expr);
}
