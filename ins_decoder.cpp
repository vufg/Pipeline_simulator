#include "ins_decoder.h"
#include "io.h"

int get_rs(int inst){
    return ((unsigned int)inst) << 6 >> 27;
}

int get_rt(int inst){
    return ((unsigned int)inst) << 11 >> 27;
}

int get_rd(int inst){
    int opcode = ((unsigned int)inst) >> 26;
    if(opcode){
    //not R type: no rd
        return -1;
    }
    return ((unsigned int)inst) << 16 >> 27;
}

int get_immediate_signed(int inst){
    return inst << 16 >> 16;
}

int get_immediate_unsigned(int inst){
    return ((unsigned int) inst) << 16 >> 16;
}

int get_shamt(int inst){
    return ((unsigned int)inst) << 21 >> 27;
}

int get_address_unsigned(int inst){
    return ((unsigned int)inst) << 6 >> 6;
}

insType get_ins_type(int inst){

    int opcode = ((unsigned int)inst) >> 26;

    ///R-Type instruction
    if(  opcode == 0x00){
        int rs, rt, rd, shamt, funct;
        rs = ((unsigned int)inst) << 6 >> 27;
        rt = ((unsigned int)inst) << 11 >> 27;
        rd = ((unsigned int)inst) << 16 >> 27;
        shamt = ((unsigned int)inst) << 21 >> 27;
        funct = ((unsigned int)inst) << 26 >> 26;

        switch (funct){
        case 0x20:
            return ADD;
            break;

        case 0x21:
            return ADDU;
            break;

        case 0x22:
            return SUB;
            break;

        case 0x24:
            return AND;
            break;

        case 0x25:
            return OR;
            break;
        ///

        case 0x26:
            return XOR;
            break;

        case 0x27:
            return NOR;
            break;

        case 0x28:
            return NAND;
            break;

        case 0x2A:
            return SLT;
            break;
        ///


        case 0x00:
            if( rt || rd || shamt){
                return SLL;
            }else{
                return NOP;
            }
            break;

        case 0x02:
            return SRL;
            break;

        case 0x03:
            return SRA;
            break;

        case 0x08:
            return JR;
            break;
        ///

        case 0x18:
            return MULT;
            break;

        case 0x19:
            return MULTU;
            break;

        case 0x10:
            return MFHI;
            break;

        case 0x12:
            return MFLO;
            break;

        default:
            ///
            printf("illegal instruction found at 0x%08X\n", pc_IF);
            quit_flag = 5;
            return NOP;

        }
    }
    /// J-Type instruction
    else if(  opcode == 0x02 || opcode == 0x03){
        if(opcode == 0x02){
            return JUMP;
        }else{
            return JAL;
        }
    }else if(opcode == 0x3f){
        return HALT;
    }

    ///I-Type instruction
    else if(opcode == 0x4 || opcode == 0x5 || (opcode >= 0x7 && opcode <= 0xA) ||
            opcode == 0x28 || opcode == 0x29 || (opcode >= 0xC && opcode <= 0xF) ||
            opcode == 0x20 || opcode == 0x21 || opcode == 0x2B || (opcode >= 0x23 && opcode <= 0x25) )
    {


        switch (opcode){
        case 0x08:
            return ADDI;
            break;

        case 0x09:
            return ADDIU;
            break;

        case 0x23:
            return LW;
            break;

        case 0x21:
            return LH;
            break;

        case 0x25:
            return LHU;
            break;
        ///

        case 0x20:
            return LB;
            break;

        case 0x24:
            return LBU;
            break;

        case 0x2B:
            return SW;
            break;

        case 0x29:
            return SH;
            break;
        ///


        case 0x28:
            return SB;
            break;

        case 0x0F:
            return LUI;
            break;

        case 0x0C:
            return ANDI;
            break;

        case 0x0D:
            return ORI;
            break;
        ///

        case 0x0E:
            return NORI;
            break;

        case 0x0A:
            return SLTI;
            break;

        case 0x04:
            return BEQ;
            break;

        case 0x05:
            return BNE;
            break;

        case 0x07:
            return BGTZ;
            break;

        //default:
        }
    }else{
            printf("illegal instruction found at 0x%08X\n", pc_IF);
            quit_flag = 5;
        }
        return NOP;


    }

