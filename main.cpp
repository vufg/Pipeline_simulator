#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "five_stages.h"
#include "simulator.h"
#include "ins_decoder.h"


//instruction type
//typedef enum {ADD,ADDU,SUB,AND,OR,XOR,NOR,NAND,SLT,SLL,SRL,SRA,JR,MULT,MULTU,MFHI,MFLO,ADDI,ADDIU,
 //             LW,LH,LHU,LB,LBU,SW,SH,SB,LUI,ANDI,ORI,NORI,SLTI,BEQ,BNE,BGTZ,JUMP,JAL,HALT,NOP} insType;

//typedef enum {NO_forwarding, EX_DM_forwarding, DM_WB_forwarding} forwardingType;

char ins_type_string[39][10] = {"ADD","ADDU","SUB","AND","OR","XOR","NOR","NAND","SLT","SLL","SRL","SRA","JR",
                               "MULT","MULTU","MFHI","MFLO","ADDI","ADDIU","LW","LH","LHU","LB","LBU","SW",
                               "SH","SB","LUI","ANDI","ORI","NORI","SLTI","BEQ","BNE","BGTZ","J","JAL","HALT","NOP"};

char forwarding_type_string[3][20] = {"", "fwd_EX-DM", "fwd_DM-WB"};



//reg and mem data
int reg[50], imemory[300], dmemory[300];
int pc_IF, pc_ID, hi, lo;


//IF buffer data
int IF_ID, ID_EX, EX_DM, DM_WB;

int IF, ID, EX, DM, WB;

//reg changed markers
int mul_changed, reg_changed[50], hi_changed, lo_changed, pc_changed;
int pre_pc;
//other global variable
int quit_flag, cycle;

//output file pointer
FILE *snapshot, *error_dump;

//variable between five stages
int IDEX_num1, IDEX_num2, EXDM_result_buffer, EXDM_mem_address_buffer, DMWB_result_buffer;
//int IDEX_num1, IDEX_num2, EXDM_EX_result, DMWB_EX_result, DMWB_DM_data;

//forwarding tag =
//0: no forwarding
//1: EX/DM forwarding
//2: DM/WB forwarding

int error_report[5] = {0};
int stall_this_cycle, IF_to_be_flushed;
forwardingType EX_rs_forwarding_tag, EX_rt_forwarding_tag, ID_rs_forwarding_tag, ID_rt_forwarding_tag;


void init(void){
    for(int i = 0; i < 50; i++){
        reg[i] = 0;
        reg_changed[i] = 1;
    }

    for(int i = 0; i < 300; i++){
        imemory[i] = 0;
        dmemory[i] = 0;
    }

    pc_IF = 0;
    pc_ID = 0;
    pre_pc = -1;

    hi = 0;
    lo = 0;

    IF = 0;
    ID = 0;
    EX = 0;
    DM = 0;
    WB = 0;

    IF_ID = 0;
    ID_EX = 0;
    EX_DM = 0;
    DM_WB = 0;

    mul_changed = 0;
    hi_changed = 1;
    lo_changed = 1;
    pc_changed = 1;

    quit_flag = 0;
    cycle = 0;
    stall_this_cycle = 0;
    IF_to_be_flushed = 0;

    IDEX_num1 = 0;
    IDEX_num2 = 0;
    EXDM_result_buffer = 0;
    EXDM_mem_address_buffer = 0;
    DMWB_result_buffer = 0;

    EX_rs_forwarding_tag = NO_forwarding;
    EX_rt_forwarding_tag = NO_forwarding;
    ID_rs_forwarding_tag = NO_forwarding;
    ID_rt_forwarding_tag = NO_forwarding;

    for(int i = 0; i < 5; i++){
        error_report[i] = 0;
    }
}

int main()
{
    init();
    input_data_file();
    open_output_file();

    simulator();

    close_output_file();
    return 0;
}
