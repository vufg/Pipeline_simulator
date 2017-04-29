#ifndef INS_DECODER_H_INCLUDED
#define INS_DECODER_H_INCLUDED
#include "io.h"
//typedef enum {ADD,ADDU,SUB,AND,OR,XOR,NOR,NAND,SLT,SLL,SRL,SRA,JR,MULT,MULTU,MFHI,MFLO,ADDI,ADDIU,
//              LW,LH,LHU,LB,LBU,SW,SH,SB,LUI,ANDI,ORI,NORI,SLTI,BEQ,BNE,BGTZ,JUMP,JAL,HALT,NOP} insType;

//typedef enum {NO_forwarding, EX_DM_forwarding, DM_WB_forwarding} forwardingType;

extern int pc_IF;
extern int quit_flag;

insType get_ins_type(int inst);
int get_rs(int inst);
int get_rt(int inst);
int get_rd(int inst);
int get_immediate_signed(int inst);
int get_immediate_unsigned(int inst);
int get_shamt(int inst);
int get_address_unsigned(int inst);



#endif // INS_DECODER_H_INCLUDED
