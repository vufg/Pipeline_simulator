#include "five_stages.h"

#include "ins_decoder.h"
#include <stdint.h>

void write_back(void){
    WB = DM_WB;
    insType optype;
    optype = get_ins_type(WB);

    //R type instruction $d = execution_result from EX stage
    if((optype >= ADD && optype <= SRA) ||
       (optype >= MFHI && optype <=MFLO)){
        register_acess(get_rd(WB), DMWB_result_buffer, 1);
    }

    //jr, mult, multu instruction: do nothing in WB stage
    if(optype >= JR && optype <= MULTU){

    }


    //I type:
    //load memory data instruction
    if(optype >= ADDI && optype <= LBU){
       register_acess(get_rt(WB), DMWB_result_buffer, 1);

    }

    //save data into memory instruction
    if(optype >= SW && optype <= SB){

    }


    //immediate instructions: lui andi ori...
    if((optype >= LUI && optype <= SLTI)){
        register_acess(get_rt(WB), DMWB_result_buffer, 1);
    }

    //branch instruction: do nothing in WB stage
    if((optype >= BEQ && optype <= BGTZ) || optype == JUMP){

    }

    //jal instruction $31 = PC + 4
    if(optype == JAL){
        register_acess(31, DMWB_result_buffer, 1);
    }

    //NOP and HALT instruction
    if(optype == NOP){

    }

    if(optype == HALT){
        quit_flag++;


    }


}

void data_memory(void){
    insType optype;

    //get data from EX/DM buffer
    DM = EX_DM;
    //printf("DM stage\n\n");
    //printf("%s \n\n", DM);
    optype = get_ins_type(DM);

    //R type instruction $d = execution_result from EX stage
    //do nothing in DM stage
    if((optype >= ADD && optype <= SRA) ||
       (optype >= MFHI && optype <=MFLO)){
           DMWB_result_buffer = EXDM_result_buffer;

    }

    //jr, mult, multu instruction: do nothing in DM stage
    if(optype >= JR && optype <= MULTU){


    }


    //I type:
    //do nothing in DM stage
    if((optype >= ADDI && optype <= ADDIU) ||
       (optype >= LUI && optype <= SLTI)){
        DMWB_result_buffer = EXDM_result_buffer;

    }

    //load data from memory instruction
    if(optype >= LW && optype <= LBU){
        int mem_data = 0;
        switch (optype){
        case LW:
            mem_data = dmemory_acess(EXDM_result_buffer, 0, 4, 0);
            break;

        case LH:
            mem_data = dmemory_acess(EXDM_result_buffer, 0, 2, 0) << 16 >> 16;
            break;

        case LHU:
            mem_data = ((unsigned int)dmemory_acess(EXDM_result_buffer, 0, 2, 0)) << 16 >> 16;
            break;

        case LB:
            mem_data = dmemory_acess(EXDM_result_buffer, 0, 1, 0) << 24 >> 24;
            break;

        case LBU:
            mem_data = ((unsigned int)dmemory_acess(EXDM_result_buffer, 0, 1, 0)) << 24 >> 24;
            break;
        default:
            break;
        }
        DMWB_result_buffer = mem_data;
    }

    //save data into memory instruction
    if(optype >= SW && optype <= SB){
        switch (optype){
        case SW:
            dmemory_acess(EXDM_mem_address_buffer, EXDM_result_buffer, 4, 1);
            break;

        case SH:
            dmemory_acess(EXDM_mem_address_buffer, EXDM_result_buffer & 0x0000FFFF, 2, 1);
            break;

        case SB:
            dmemory_acess(EXDM_mem_address_buffer, EXDM_result_buffer & 0x000000FF, 1, 1);
            break;

        default:
            break;
        }

    }

    //branch instruction
    if((optype >= BEQ && optype <= BGTZ) || optype == JUMP){

    }

    //jal instruction
    if(optype == JAL){
        DMWB_result_buffer = EXDM_result_buffer;

    }

    //NOP and HALT instruction
    if(optype == NOP){

    }

    if(optype == HALT){
        quit_flag++;


    }

    //store data into DM/WB buffer
    DM_WB = EX_DM;// DW_WB = DM



}

void excution(void){
    int rsrt_array[39][2] = {{1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1},
                             {0,1}, {0,1}, {0,1},
                             //jr: do nothing in EX stage
                             {0,0},
                             //MULT MULTU
                             {1,1}, {1,1},
                             {0,0}, {0,0},
                             //I type instruction
                             {1,0}, {1,0}, {1,0}, {1,0}, {1,0}, {1,0}, {1,0},
                             //SW SH SB
                             {1,0}, {1,0}, {1,0},
                             {0,0},
                             {1,0}, {1,0}, {1,0}, {1,0},
                             //BEQ BNE BGTZ
                             {0,0}, {0,0}, {0,0},
                             {0,0}, {0,0}, {0,0}, {0,0}
                             };
    int rs, rt, shamt;
    insType optype;

    //get data from ID/EX buffer
    EX = ID_EX;
    optype = get_ins_type(EX);
    rs = get_rs(EX);
    rt = get_rt(EX);
    shamt = get_shamt(EX);


    //data forwarding
    //adjust the number in IDEX_num1 and IDEX_num2

    //check DM_WB_forwarding first
    //than check EX_DM_forwarding, most recently instructions, latter check

    //check DM_WB_forwarding first
    if(rsrt_array[optype][0] &&
        ((rs == get_rd(WB) && get_ins_type(WB) >= ADD && get_ins_type(WB) <= SRA) ||
        (rs == get_rd(WB) && get_ins_type(WB) >= MFHI && get_ins_type(WB) <= MFLO) ||
        (rs == get_rt(WB) && get_ins_type(WB) >= ADDI && get_ins_type(WB) <= LBU) ||
        (rs == get_rt(WB) && get_ins_type(WB) >= LUI && get_ins_type(WB) <= SLTI) ||
        (rs == 31 && get_ins_type(WB) == JAL) )){
            EX_rs_forwarding_tag = DM_WB_forwarding;
            IDEX_num1 = reg[rs];

    }
    if(rsrt_array[optype][1] &&
        ((rt == get_rd(WB) && get_ins_type(WB) >= ADD && get_ins_type(WB) <= SRA) ||
        (rt == get_rd(WB) && get_ins_type(WB) >= MFHI && get_ins_type(WB) <= MFLO) ||
        (rt == get_rt(WB) && get_ins_type(WB) >= ADDI && get_ins_type(WB) <= LBU) ||
        (rt == get_rt(WB) && get_ins_type(WB) >= LUI && get_ins_type(WB) <= SLTI) ||
        (rt == 31 && get_ins_type(WB) == JAL) )){
        EX_rt_forwarding_tag = DM_WB_forwarding;
        IDEX_num2 = reg[rt];
    }

    //check EX_DM_forwarding
    if(rsrt_array[optype][0] &&
        ((rs == get_rd(DM) && get_ins_type(DM) >= ADD && get_ins_type(DM) <= SRA) ||
        (rs == get_rd(DM) && get_ins_type(DM) >= MFHI && get_ins_type(DM) <= MFLO) ||
        (rs == get_rt(DM) && get_ins_type(DM) >= ADDI && get_ins_type(DM) <= ADDIU) ||
        //LW to LBU is impossible
        (rs == get_rt(DM) && get_ins_type(DM) >= LUI && get_ins_type(DM) <= SLTI) ||
        (rs == 31 && get_ins_type(DM) == JAL) )){
        EX_rs_forwarding_tag = EX_DM_forwarding;
        IDEX_num1 = DMWB_result_buffer;
    }

    if(rsrt_array[optype][1] &&
        ((rt == get_rd(DM) && get_ins_type(DM) >= ADD && get_ins_type(DM) <= SRA) ||
        (rt == get_rd(DM) && get_ins_type(DM) >= MFHI && get_ins_type(DM) <= MFLO) ||
        (rt == get_rt(DM) && get_ins_type(DM) >= ADDI && get_ins_type(DM) <= ADDIU) ||
        //LW to LBU is impossible
        (rt == get_rt(DM) && get_ins_type(DM) >= LUI && get_ins_type(DM) <= SLTI) ||
        (rs == 31 && get_ins_type(DM) == JAL) )){
        EX_rt_forwarding_tag = EX_DM_forwarding;
        IDEX_num2 = EXDM_result_buffer;
    }


    switch (optype){
    //R type instruction
    case ADD:
        EXDM_result_buffer = IDEX_num1 + IDEX_num2;
        ///overflow detection
        overflow_f(IDEX_num1, IDEX_num2);
        break;

    case ADDU:
        EXDM_result_buffer = IDEX_num1 + IDEX_num2;
        break;

    case SUB:
        EXDM_result_buffer = IDEX_num1 - IDEX_num2;
        ///overflow detection
        overflow_f(IDEX_num1, -IDEX_num2);
        break;

    case AND:
        EXDM_result_buffer = IDEX_num1 & IDEX_num2;
        break;

    case OR:
        EXDM_result_buffer = IDEX_num1 | IDEX_num2;
        break;

    case XOR:
        EXDM_result_buffer = IDEX_num1 ^ IDEX_num2;
        break;

    case NOR:
        EXDM_result_buffer = ~(IDEX_num1 | IDEX_num2);

        break;

    case NAND:
        EXDM_result_buffer = ~(IDEX_num1 & IDEX_num2);

        break;

    case SLT:
        if(IDEX_num1 < IDEX_num2){
            EXDM_result_buffer = 1;
        }else{
            EXDM_result_buffer = 0;
        }

        break;

    case SLL:
        EXDM_result_buffer = IDEX_num2 << shamt;

        break;

    case SRL:
        EXDM_result_buffer = (unsigned int)IDEX_num2 >> shamt;
        break;

    case SRA:
        EXDM_result_buffer = IDEX_num2 >> shamt;

    case JR:
        //do nothing
        break;

    case MULT:
        if(mul_changed){
            error_report[1] = 1;
             //fprintf(error_dump , "In cycle %d: Overwrite HI-LO registers\n", cycle);
	    }
	    mul_changed = 1;
	    long long ans;
	    int tmp;
	    ans = IDEX_num1 * IDEX_num2;
	    tmp = ans >> 32;
	    hi_access( tmp, 1);
	    tmp = ans;
	    lo_access( tmp, 1);
        break;

    case MULTU:
        if(mul_changed){
            error_report[1] = 1;
	    }
	    mul_changed = 1;
	    int64_t a,b,c,d;
        int e,f;
        a = (unsigned int)IDEX_num1;
        b = (unsigned int)IDEX_num2;
        c = ((int64_t)( a * b)) >> 32;
        d = ((int64_t)( a * b)) << 32 >> 32;
        e = c;
        f = d;
	    hi_access(e,1);
	    lo_access(f,1);
        break;

    case MFHI:
        EXDM_result_buffer = hi_access(0,0);
        break;

    case MFLO:
        EXDM_result_buffer = lo_access(0,0);
        break;


    //I type instruction
    case ADDI:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        ///overflow detection
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case ADDIU:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LW:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));

        break;

    case LH:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case LHU:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case LB:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case LBU:
        EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case SW:
        EXDM_result_buffer = IDEX_num2;
        EXDM_mem_address_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));

        break;

    case SH:
        EXDM_result_buffer = IDEX_num2;
        EXDM_mem_address_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case SB:
        EXDM_result_buffer = IDEX_num2;
        EXDM_mem_address_buffer = IDEX_num1 + get_immediate_signed(EX);
        overflow_f(IDEX_num1, get_immediate_signed(EX));
        break;

    case LUI:
        EXDM_result_buffer = get_immediate_unsigned(EX) << 16;

        break;

    case ANDI:
        EXDM_result_buffer = IDEX_num1 & get_immediate_unsigned(EX);

        break;

    case ORI:
        EXDM_result_buffer = IDEX_num1 | get_immediate_unsigned(EX);

        break;

    case NORI:
        EXDM_result_buffer = ~(IDEX_num1 | get_immediate_unsigned(EX));

        break;

    case SLTI:
        if(IDEX_num1 < get_immediate_signed(EX)){
            EXDM_result_buffer = 1;
        }else{
            EXDM_result_buffer = 0;

        }

        break;

    case BEQ:

        break;

    case BNE:

        break;

    case BGTZ:

        break;

    case JUMP:

        break;

    case JAL:
        EXDM_result_buffer = IDEX_num1;

        break;

    case HALT:
        quit_flag++;
        break;

    case NOP:

        break;




    }

    //store data into DM/WB buffer
    EX_DM = EX;





}

void instruction_decoder(void){

    int rsrt_array[39][2] = {{1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1},
                             {0,1}, {0,1}, {0,1},
                             {1,0},
                             {1,1}, {1,1},
                             {0,0}, {0,0},
                             {1,0}, {1,0}, {1,0}, {1,0}, {1,0}, {1,0}, {1,0},
                             {1,0}, {1,0}, {1,0},
                             {0,0},
                             {1,0}, {1,0}, {1,0}, {1,0},
                             //beq bne bgtz
                             {1,1}, {1,1}, {1,0},
                             {0,0}, {0,0}, {0,0}, {0,0}
                             };


    int rs, rt;

    // only for conditional branch instructions
    int reg_rs, reg_rt;
    insType optype;


    //get data from ID/EX buffer
    ID = IF_ID;
    optype = get_ins_type(ID);
    rs = get_rs(ID);
    rt = get_rt(ID);

    //detect stall
    /*
    if(cycle == 2){
        printf("%0x%08X\n", ID);
        printf("rs: %d rt: %d\n", rs, rt);
        printf("%0x%08X\n", EX);
        printf("%d \n", get_ins_type()
    }
    */
    if(rsrt_array[optype][0] &&
       rs == get_rt(EX) && get_ins_type(EX) >= LW && get_ins_type(EX) <= LBU ){
        stall_this_cycle = 1;

    }else if(rsrt_array[optype][1] &&
       rt == get_rt(EX) && get_ins_type(EX) >= LW && get_ins_type(EX) <= LBU ){
        stall_this_cycle = 1;

    //detect stall in branch instructions
    }else if(optype >= BEQ && optype <= BGTZ){
        if(rsrt_array[optype][0] &&
          ((rs == get_rd(EX) && get_ins_type(EX) >= ADD && get_ins_type(EX) <= SRA) ||
           (rs == get_rd(EX) && get_ins_type(EX) >= MFHI && get_ins_type(EX) <= MFLO) ||
           (rs == get_rt(EX) && get_ins_type(EX) >= ADDI && get_ins_type(EX) <= LBU) ||
           (rs == get_rt(EX) && get_ins_type(EX) >= LUI && get_ins_type(EX) <= SLTI) ||
           (rs == 31 && get_ins_type(EX) == JAL ) ||
           (rs == get_rt(DM) && get_ins_type(DM) >= LW && get_ins_type(DM) <= LBU))){
           //stall is detected
            stall_this_cycle = 1;
           }

        if(rsrt_array[optype][1] &&
          ((rt == get_rd(EX) && get_ins_type(EX) >= ADD && get_ins_type(EX) <= SRA) ||
           (rt == get_rd(EX) && get_ins_type(EX) >= MFHI && get_ins_type(EX) <= MFLO) ||
           (rt == get_rt(EX) && get_ins_type(EX) >= ADDI && get_ins_type(EX) <= LBU) ||
           (rt == get_rt(EX) && get_ins_type(EX) >= LUI && get_ins_type(EX) <= SLTI) ||
           (rt == 31 && get_ins_type(EX) == JAL ) ||
           (rt == get_rt(DM) && get_ins_type(DM) >= LW && get_ins_type(DM) <= LBU))){
           //stall is detected
            stall_this_cycle = 1;
           }
    }

    if(stall_this_cycle == 0){
        reg_rs = reg[rs];
        reg_rt = reg[rt];

        //forwarding in ID stage
        if(optype >= BEQ && optype <= BGTZ){
            if(rsrt_array[optype][0] &&
                ((rs == get_rd(DM) && get_ins_type(DM) >= ADD && get_ins_type(DM) <= SRA) ||
                (rs == get_rd(DM) && get_ins_type(DM) >= MFHI && get_ins_type(DM) <= MFLO) ||
                (rs == get_rt(DM) && get_ins_type(DM) >= ADDI && get_ins_type(DM) <= ADDIU) ||
                (rs == get_rt(DM) && get_ins_type(DM) >= LUI && get_ins_type(DM) <= SLTI) ||
                (rs == 31 && get_ins_type(DM) == JAL ))){
                    ID_rs_forwarding_tag = EX_DM_forwarding;
                    reg_rs = DMWB_result_buffer;
                }

            if(rsrt_array[optype][1] &&
                ((rt == get_rd(DM) && get_ins_type(DM) >= ADD && get_ins_type(DM) <= SRA) ||
                (rt == get_rd(DM) && get_ins_type(DM) >= MFHI && get_ins_type(DM) <= MFLO) ||
                (rt == get_rt(DM) && get_ins_type(DM) >= ADDI && get_ins_type(DM) <= ADDIU) ||
                (rt == get_rt(DM) && get_ins_type(DM) >= LUI && get_ins_type(DM) <= SLTI) ||
                (rt == 31 && get_ins_type(DM) == JAL ))){
                    ID_rt_forwarding_tag = EX_DM_forwarding;
                    reg_rt = DMWB_result_buffer;
                }
        }
    }






    if(stall_this_cycle){
        //insert NOP
        ID_EX = 0x00000000;
        return ;
    }

    //printf("cycle: %d\n", cycle);




    pc_ID = pc_IF + 4;

    //instruction decoder
    switch (optype){
    //R type instruction
    case ADD:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case ADDU:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case SUB:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case AND:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case OR:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case XOR:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case NOR:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case NAND:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case SLT:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case SLL:
        IDEX_num1 = reg_rt;
        break;

    case SRL:
        IDEX_num1 = reg_rt;
        break;

    case SRA:
        IDEX_num1 = reg_rt;

    case MULT:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case MULTU:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        break;

    case MFHI:
        EXDM_result_buffer = hi_access(0,0);
        break;

    case MFLO:
        EXDM_result_buffer = lo_access(0,0);
        break;



    //I type instruction
    case ADDI:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        ///overflow detection

        break;

    case ADDIU:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_unsigned(EX);

        break;

    case LW:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LH:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LHU:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LB:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LBU:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case SW:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case SH:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);
        break;

    case SB:
        IDEX_num1 = reg_rs;
        IDEX_num2 = reg_rt;
        //EXDM_result_buffer = IDEX_num1 + get_immediate_signed(EX);

        break;

    case LUI:

        //EXDM_result_buffer = get_immediate_unsigned(EX) << 16;

        break;

    case ANDI:
        IDEX_num1 = reg_rs;

        //EXDM_result_buffer = IDEX_num1 & get_immediate_unsigned(EX);

        break;

    case ORI:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = IDEX_num1 | get_immediate_unsigned(EX);

        break;

    case NORI:
        IDEX_num1 = reg_rs;
        //EXDM_result_buffer = ~(IDEX_num1 & get_immediate_unsigned(EX));

        break;

    case SLTI:
        IDEX_num1 = reg_rs;
        /*if(IDEX_num1 < get_immediate_signed(EX)){
            EXDM_result_buffer = 1;
        }else{
            EXDM_result_buffer = 0;

        }*/
        break;


    ///instructions related to PC
    case BEQ:
        if(reg_rs == reg_rt){
            int target_pc = pc_ID + (get_immediate_signed(ID) << 2) - 4;
            overflow_f(pc_ID, get_immediate_signed(ID) << 2);
            if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
            }
            pc_ID = target_pc;
        }

        break;

    case BNE:
        if(reg_rs != reg_rt){
            int target_pc = pc_ID + (get_immediate_signed(ID) << 2) - 4;
            overflow_f(pc_ID, get_immediate_signed(ID) << 2);
            if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
            }
            pc_ID = target_pc;
        }

        break;

    case BGTZ:
        if(reg_rs > 0){
            int target_pc = pc_ID + (get_immediate_signed(ID) << 2) - 4;
            overflow_f(pc_ID, get_immediate_signed(ID) << 2);
            if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
            }
            pc_ID = target_pc;
        }

        break;

    case JUMP:
        int target_pc;
        target_pc = (pc_ID >> 28 << 28) | (get_address_unsigned(ID) << 2);
        if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
        }
        pc_ID = target_pc;

        break;

    case JAL:
        IDEX_num1 = pc_ID;
        //int target_pc;
        target_pc = (pc_ID >> 28 << 28) | (get_address_unsigned(ID) << 2);
        if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
        }
        pc_ID = target_pc;
        //EXDM_result_buffer = IDEX_num1;

        break;

    case JR:
        //int target_pc;
        target_pc = register_acess(rs, 0, 0);
        if(target_pc != pc_IF){
                IF_to_be_flushed = 1;
        }
        pc_ID = target_pc;
        break;

    case HALT:
        quit_flag++;
        break;

    case NOP:

        break;

    }

    //store data into DM/WB buffer
    ID_EX = IF_ID;// DW_WB = DM

}

void instruction_fetch(void){
    IF = imemory[pc_IF / 4];
    if(get_ins_type(IF) == HALT){
        quit_flag++;
    }
    if(stall_this_cycle == 0){
        IF_ID = IF;
    }

    return ;
}
