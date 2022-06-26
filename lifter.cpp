#include "lifter.h"

ExprId cond_branch(BinaryNinja::LowLevelILFunction &il, Instruction &inst, ExprId condition, uint64_t size) {
    ExprId dest = il.Add(64, il.Const(64, il.GetCurrentAddress()),
                         il.SignExtend(64, il.Const(64, inst.imm)));

    BNLowLevelILLabel *trueLabel = il.GetLabelForAddress(il.GetArchitecture(), il.GetCurrentAddress() + inst.imm);
    auto falseLabel = il.GetLabelForAddress(il.GetArchitecture(), il.GetCurrentAddress() + 4);

    return il.AddInstruction(il.If(condition, reinterpret_cast<BNLowLevelILLabel &>(trueLabel),
                                   reinterpret_cast<BNLowLevelILLabel &>(falseLabel)));
}

ExprId store_helper(BinaryNinja::LowLevelILFunction &il, Instruction &inst, uint64_t size) {
    ExprId addr = il.Add(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm)));
    ExprId val = il.Register(64, inst.rs2);
    return il.Store(size, addr, val);
};

ExprId load_helper(BinaryNinja::LowLevelILFunction &il, Instruction &inst, uint64_t size) {
    ExprId addr = il.Add(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm)));
    return il.SetRegister(64, inst.rd, il.SignExtend(64, il.Load(size, addr)));
}

bool liftToLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, BinaryNinja::LowLevelILFunction &il) {
    Instruction inst = Disassembler::disasm(data, addr);
    ExprId expr = il.Unimplemented();
    switch (inst.mnemonic) {
        case ADDI:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Add(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm))));
            break;
        case ADD:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Add(32, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case SUB:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Sub(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case AUIPC:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Add(64, il.Const(64, inst.imm << 12), il.Const(64, addr)));
            break;
        case JAL:
            break;
        case JALR:
            break;
        case BEQ:
            break;
        case BNE: {
            ExprId cond = il.CompareNotEqual(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2));
            expr = cond_branch(il, inst, cond, 64);
            break;
        }
        case BLT:
            break;
        case BGE:
            break;
        case BLTU:
            break;
        case BGEU:
            break;
        case LB:
            expr = load_helper(il, inst, 8);
            break;
        case LH:
            expr = load_helper(il, inst, 16);
            break;
        case LBU:
            break;
        case LHU:
            break;
        case LWU:
            break;
        case LW:
            expr = load_helper(il, inst, 32);
            break;
        case LD:
            expr = load_helper(il, inst, 64);
            break;
        case SB:
            expr = store_helper(il, inst, 8);
            break;
        case SH:
            expr = store_helper(il, inst, 16);
            break;
        case SW:
            expr = store_helper(il, inst, 32);
            break;
        case SD:
            expr = store_helper(il, inst, 64);
            break;
        case SLTI:
            break;
        case SLTIU:
            break;
        case XORI:
            expr = il.SetRegister(64, inst.rd,
                                  il.Xor(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm))));
            break;
        case ORI:
            expr = il.SetRegister(64, inst.rd,
                                  il.Or(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm))));
            break;
        case ANDI:
            expr = il.SetRegister(64, inst.rd,
                                  il.And(64, il.Register(64, inst.rs1), il.SignExtend(64, il.Const(12, inst.imm))));
            break;
        case SLLI:
            break;
        case SRLI:
            break;
        case SLL:
            break;
        case SLT:
            break;
        case SLTU:
            break;
        case XOR:
            expr = il.SetRegister(64, inst.rd, il.Xor(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case SRL:
            break;
        case SRA:
            break;
        case OR:
            expr = il.SetRegister(64, inst.rd, il.Or(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case AND:
            expr = il.SetRegister(64, inst.rd, il.And(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case FENCE:
            break;
        case ECALL:
            break;
        case EBREAK:
            break;
        case SRAI:
            break;
        case ADDIW:
            break;
        case SLLIW:
            break;
        case SRLIW:
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
            expr = il.Jump(il.Const(64, inst.imm));
            break;
        case LI:
            expr = il.SetRegister(64, inst.rd, il.ConstPointer(64, inst.imm));
            break;
        case RET:
            expr = il.Return(il.Register(64, inst.rs1));
            break;
        default:
            break;
    }
    il.AddInstruction(expr);
    return true;
}

