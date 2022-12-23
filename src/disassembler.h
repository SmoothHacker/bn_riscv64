#ifndef BN_RISCV_ARCH_DISASSEMBLER_H
#define BN_RISCV_ARCH_DISASSEMBLER_H

#include <cstdint>
#include <cstdlib>
#include <string>

#include "binaryninjaapi.h"

enum Registers {
	Zero = 0,
	// x1 - return address (caller saved)
	ra,
	// x2 - stack pointer (callee saved)
	sp,
	// x3 - global pointer
	gp,
	// x4 - threat pointer
	tp,
	// x5-7 - temporaries (caller saved)
	t0,
	t1,
	t2,
	// x8 - saved register / frame pointer (caller saved)
	s0,
	// x9 - saved register
	s1,
	// x10-x11 - first function argument and return value (caller saved)
	a0,
	a1,
	// x12-17 - function arguments (caller saved)
	a2,
	a3,
	a4,
	a5,
	a6,
	a7,
	// x18-27 - saved registers (caller saved)
	s2,
	s3,
	s4,
	s5,
	s6,
	s7,
	s8,
	s9,
	s10,
	s11,
	// x28-31 - temporaries
	t3,
	t4,
	t5,
	t6,
	// pc (caller saved)
	pc,
};

static const char* registerNames[] = {
	"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",
	"a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
	"s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6", "pc"
};

static const char* instrNames[] = {
	"lui", "auipc", "jal", "jalr", "beq", "bne", "blt", "bge",
	"bltu", "bgeu", "lb", "lh", "lw", "lbu", "lhu", "sb",
	"sh", "sw", "addi", "slti", "sltiu", "xori", "ori", "andi",
	"slli", "srli", "add", "sub", "sll", "slt", "sltu", "xor",
	"srl", "sra", "or", "and", "fence", "ecall", "ebreak", "lwu",
	"ld", "sd", "srai", "addiw", "slliw", "srliw", "sraiw", "addw",
	"subw", "sllw", "srlw", "sraw", "j", "li", "ret", "mv", "jr"
};

enum InstrName {
	UNSUPPORTED = -1,
	// RV32I Base
	LUI = 0,
	AUIPC,
	JAL,
	JALR,
	BEQ,
	BNE,
	BLT,
	BGE,
	BLTU,
	BGEU,
	LB,
	LH,
	LW,
	LBU,
	LHU,
	SB,
	SH,
	SW,
	ADDI,
	SLTI,
	SLTIU,
	XORI,
	ORI,
	ANDI,
	SLLI,
	SRLI,
	ADD,
	SUB,
	SLL,
	SLT,
	SLTU,
	XOR,
	SRL,
	SRA,
	OR,
	AND,
	FENCE,
	ECALL,
	EBREAK,
	// RV64I Base
	LWU,
	LD,
	SD,
	SRAI,
	ADDIW,
	SLLIW,
	SRLIW,
	SRAIW,
	ADDW,
	SUBW,
	SLLW,
	SRLW,
	SRAW,
	// Pseudo-instructions for RV64I
	J,
	LI,
	RET,
	MV,
	JR
};

enum InstrType {
	Error = -1,
	Rtype,
	Itype,
	Stype,
	Btype,
	Utype,
	Jtype
};

class Instruction {
public:
	InstrType type = Error;
	InstrName mnemonic = InstrName::UNSUPPORTED;
	size_t rd = 0;
	size_t rs1 = 0;
	size_t rs2 = 0;
	uint32_t funct7 = 0;
	uint32_t funct3 = 0;
	int64_t imm = 0;
};

class Disassembler {
	static Instruction implRtype(uint32_t insword);

	static Instruction implItype(uint32_t insword);

	static Instruction implStype(uint32_t insword);

	static Instruction implBtype(uint32_t insword);

	static Instruction implUtype(uint32_t instr);

	static Instruction implJtype(uint32_t insword);

public:
	static Instruction disasm(const uint8_t* data, uint64_t addr);
};

#endif // BN_RISCV_ARCH_DISASSEMBLER_H
