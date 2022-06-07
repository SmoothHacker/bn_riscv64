#include "lifter.h"

bool liftToLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, BinaryNinja::LowLevelILFunction &il) {
    Instruction inst = Disassembler::disasm(data, addr);
    ExprId expr = il.Unimplemented();
    switch (inst.mnemonic) {
        case LW:
            expr = il.SetRegister(32, inst.rd, il.ConstPointer(64, inst.imm));
            break;
        case ADDI:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Add(64, il.Register(64, inst.rs1), il.Const(64, inst.imm)));
            break;
        case ADD:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Add(32, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
        case SUB:
            expr = il.SetRegister(64, il.Register(64, inst.rd),
                                  il.Sub(64, il.Register(64, inst.rs1), il.Register(64, inst.rs2)));
            break;
            /* case AUIPC:
                 break;
             case JAL:
                 break;
             case JALR:
                 break;
             case BEQ:
                 break;
             case BNE:
                 break;
             case BLT:
                 break;
             case BGE:
                 break;
             case BLTU:
                 break;
             case BGEU:
                 break;
             case LB:
                 break;
             case LH:
                 break;
             case LBU:
                 break;
             case LHU:
                 break;
             case SB:
                 break;
             case SH:
                 break;
             case SW:
                 break;
             case SLTI:
                 break;
             case SLTIU:
                 break;*/
        case XORI:
            expr = il.Xor(64, inst.rs1, inst.rd);
            break;
            /*case ORI:
                break;
            case ANDI:
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
                break;
            case SRL:
                break;
            case SRA:
                break;
            case OR:
                break;
            case AND:
                break;
            case FENCE:
                break;
            case ECALL:
                break;
            case EBREAK:
                break;
            case LWU:
                break;
            case LD:
                break;*/
        case SD:
            expr = il.Add(8, il.Register(64, inst.rs1), il.Register(64, inst.rs2));
            break;
            /*case SRAI:
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
                break;*/
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
};

