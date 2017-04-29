#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED
#include <stdio.h>
typedef enum {ADD,ADDU,SUB,AND,OR,XOR,NOR,NAND,SLT,SLL,SRL,SRA,JR,MULT,MULTU,MFHI,MFLO,ADDI,ADDIU,
              LW,LH,LHU,LB,LBU,SW,SH,SB,LUI,ANDI,ORI,NORI,SLTI,BEQ,BNE,BGTZ,JUMP,JAL,HALT,NOP} insType;

typedef enum {NO_forwarding, EX_DM_forwarding, DM_WB_forwarding} forwardingType;

//reg and mem data
extern int reg[50], imemory[300], dmemory[300];
extern int pc_IF, pc_ID, hi, lo;

//IF buffer data
extern int IF_ID, ID_EX, EX_DM, DM_WB;
extern int IF, ID, EX, DM, WB;

//reg changed markers
extern int mul_changed, reg_changed[50], hi_changed, lo_changed, pc_changed;

//other global variable
extern int quit_flag, cycle;

//output file pointer
extern FILE *snapshot, *error_dump;

//variable between five stages
extern int IDEX_num1, IDEX_num2, EXDM_result_buffer, EXDM_mem_address_buffer, DMWB_result_buffer;
extern forwardingType EX_rs_forwarding_tag, EX_rt_forwarding_tag, ID_rs_forwarding_tag, ID_rt_forwarding_tag;

extern int error_report[5];
extern int stall_this_cycle, IF_to_be_flushed;


extern char ins_type_string[39][10];

extern char forwarding_type_string[3][20];


int dmemory_acess(int address, int value, int len, int write_enable);
int register_acess(int address, int value, int write_enable);
int pc_access(int value, int write_enable);
int hi_access(int value, int write_enable);
int lo_access(int value, int write_enable);


void output_errordump(void);

void output_snapshot_ins(void);
void output_snapshot_reg(void);

int overflow_f(int a, int b);
int lo_access(int value, int write_enable);
int hi_access(int value, int write_enable);
int register_acess(int address, int value, int write_enable);
int dmemory_acess(int address, int value, int len, int write_enable);
void close_output_file(void);
void open_output_file(void);
void input_data_file(void);
unsigned int change_endian(unsigned int num);
#endif // IO_H_INCLUDED
