#include <stdio.h>
#include <string.h>
//#include "io.h"
//#include "ins_decoder.h"


#include "io.h"
//#include "five_stages.h"
//#include "simulator.h"
#include "ins_decoder.h"


unsigned int change_endian(unsigned int num){
    unsigned int ans = 0;
    for(unsigned int i = 0; i < 4; i++){
        ans = ans | (num << (i << 3) >> 24 << (i << 3));
    }
    return ans;
}

void input_data_file(void){

    FILE *iimage_file, *dimage_file;
    unsigned int inst, num_dimage, num_iimage;

    for(int i = 0; i < 32; i++)
        reg_changed[i] = 1;
    hi_changed = 1;
    lo_changed = 1;
    pc_changed = 1;

    iimage_file = fopen("iimage.bin", "rb+");
    dimage_file = fopen("dimage.bin", "rb+");


    fread(&inst, sizeof(unsigned int), 1, dimage_file);
    register_acess(29, change_endian(inst), 1);

    fread(&inst, sizeof(unsigned int), 1, dimage_file);
    num_dimage = change_endian(inst);


    for(unsigned int ii = 0; ii < num_dimage; ii++){
        fread(&inst, sizeof(unsigned int), 1, dimage_file);
        dmemory_acess(ii * 4, change_endian(inst), 4, 1);
    }

    fread(&inst, sizeof(unsigned int), 1, iimage_file);
    pc_IF = change_endian(inst);
    fread(&inst, sizeof(unsigned int), 1, iimage_file);
    num_iimage = change_endian(inst);
    for(unsigned int ii = 0; ii < num_iimage; ii++){
        fread(&inst, sizeof(unsigned int), 1, iimage_file);
        imemory[pc_IF/4 + ii] = change_endian(inst);
    }

    fclose(iimage_file);
    fclose(dimage_file);

    return ;
}

void open_output_file(void){
    snapshot = fopen("snapshot.rpt", "w+");
    error_dump = fopen("error_dump.rpt", "w+");
}

void close_output_file(void){
    fclose(snapshot);
    fclose(error_dump);
}

//no matter many bytes dmemory_access get or return
//always in unsigned int form
//all 0 in the left for half word or byte operation
int dmemory_acess(int address, int value, int len, int write_enable){
    if((address < 0 || address + len - 1 > 1023)||(address % len)){
        if(address < 0 || address + len - 1 > 1023){
            error_report[2] = 1;
            //fprintf(error_dump , "In cycle %d: Address Overflow\n", cycle);
        }
        if(address % len){
            error_report[3] = 1;
            //fprintf(error_dump , "In cycle %d: Misalignment Error\n", cycle);
        }
        quit_flag = 5;
        return 0;
    }

    int tmp = dmemory[address/4];


    if(len == 4) {
        if(write_enable){
            tmp = value;
            dmemory[address/4] = tmp;
        }else{
            return tmp;
        }


    }else if(len == 2){
        if(write_enable)
        {
            if(address % 4 == 2){
                tmp = tmp & 0xFFFF0000;
                tmp = tmp | (value & 0x0000FFFF);
            }else{
                tmp = tmp & 0x0000FFFF;
                tmp = tmp | (value << 16);
            }
            dmemory[address/4] = tmp;
        }else
        {
            if(address % 4 == 2){
                return ((unsigned int)tmp) << 16 >> 16;
            }else{
                return ((unsigned int)tmp) >> 16;
            }
        }

    }else if(len == 1){
        int shift = address % 4;
        if(write_enable){
            int mask = 0xFF000000;
            mask = mask >> (shift * 8);
            mask = ~mask;
            tmp = tmp & mask;
            value = value & 0x000000FF;
            tmp = tmp | (value << ((3 - shift) * 8));
            dmemory[address/4] = tmp;
        }else{
            return ((unsigned int)tmp) << ( shift * 8) >> 24;
        }
    }

    return 0;
}

int register_acess(int address, int value, int write_enable){

    if(address == 0 && write_enable){
        error_report[0] = 1;
        return 0;
    }
    if(write_enable){
        //printf("cycle: %d\n",cycle);
        //printf("address: %d\n",address);
        if(reg[address] != value){
            reg_changed[address] = 1;
            reg[address] = value;
        }
        return 0;
    }else{
        return reg[address];
    }

}


int hi_access(int value, int write_enable){
    if(write_enable){
        if(hi != value){
            hi_changed = 1;
            hi = value;
        }
    }else{
        mul_changed = 0;
        return hi;
    }
    return 0;
}

int lo_access(int value, int write_enable){
    if(write_enable){
        if(lo != value){
            lo = value;
            lo_changed = 1;
        }
    }else{
        mul_changed = 0;
        return lo;
    }
    return 0;
}


int overflow_f(int a, int b){

        int s = (a + b) >> 31;
        a = a >> 31;
        b = b >> 31;
        if( !(a ^ b) && (s ^ a) ){
            error_report[4] = 1;
            //fprintf(error_dump , "In cycle %d: Number Overflow\n", cycle);
            return 1;
        }
        return 0;
}


void output_snapshot_reg(void){
    fprintf(snapshot, "cycle %d\n", cycle);
    int no_change = 1;
    for(int i = 0; i < 32; i++)
        if(reg_changed[i]){
            fprintf(snapshot, "$%.2d: 0x%08X\n", i, reg[i]);
            reg_changed[i] = 0;
            no_change = 0;
        }
    if(hi_changed){
        fprintf(snapshot, "$HI: 0x%08X\n", hi);
        hi_changed = 0;
        no_change = 0;
    }

    if(lo_changed){
        fprintf(snapshot, "$LO: 0x%08X\n", lo);
        lo_changed = 0;
        no_change = 0;
    }

    if(pc_changed || no_change){
        fprintf(snapshot, "PC: 0x%08X\n", pc_IF);
        pc_changed = 0;
    }
}

void output_snapshot_ins(void){
    //IF stage
    fprintf(snapshot, "IF: 0x%08X", IF);
    if(stall_this_cycle){
        fprintf(snapshot, " to_be_stalled");
    }else if(IF_to_be_flushed){
        fprintf(snapshot, " to_be_flushed");
    }
    fprintf(snapshot, "\n");



    //ID stage
    fprintf(snapshot, "ID: %s", ins_type_string[get_ins_type(ID)]);

    if(stall_this_cycle){
        fprintf(snapshot, " to_be_stalled");
    }

    if(ID_rs_forwarding_tag == EX_DM_forwarding){
        fprintf(snapshot, " %s_rs_$", forwarding_type_string[ID_rs_forwarding_tag]);
        if(get_ins_type(DM) <= MFLO){
            //J-type instruction
            fprintf(snapshot, "%d", get_rd(DM));
        }else{
            //I-type instruction
            fprintf(snapshot, "%d", get_rt(DM));
        }
    }

    if(ID_rt_forwarding_tag == EX_DM_forwarding){
        fprintf(snapshot, " %s_rt_$", forwarding_type_string[ID_rt_forwarding_tag]);
        if(get_ins_type(DM) <= MFLO){
            //J-type instruction
            fprintf(snapshot, "%d", get_rd(DM));
        }else{
            //I-type instruction
            fprintf(snapshot, "%d", get_rt(DM));
        }
    }
    fprintf(snapshot, "\n");

    stall_this_cycle = 0;
    IF_to_be_flushed = 0;
    ID_rs_forwarding_tag = NO_forwarding;
    ID_rt_forwarding_tag = NO_forwarding;


    //EX stage
    fprintf(snapshot, "EX: %s", ins_type_string[get_ins_type(EX)]);
    if(EX_rs_forwarding_tag > NO_forwarding){
        fprintf(snapshot, " %s_rs_$", forwarding_type_string[EX_rs_forwarding_tag]);
        fprintf(snapshot, "%d", get_rs(EX));
    }

    if(EX_rt_forwarding_tag > NO_forwarding){
        fprintf(snapshot, " %s_rt_$", forwarding_type_string[EX_rt_forwarding_tag]);
        fprintf(snapshot, "%d", get_rt(EX));
    }
    fprintf(snapshot, "\n");
    EX_rs_forwarding_tag = NO_forwarding;
    EX_rt_forwarding_tag = NO_forwarding;


    //DM stage
    fprintf(snapshot, "DM: %s", ins_type_string[get_ins_type(DM)]);
    fprintf(snapshot, "\n");

    //WB stage
    fprintf(snapshot, "WB: %s", ins_type_string[get_ins_type(WB)]);
    fprintf(snapshot, "\n");

    fprintf(snapshot, "\n\n");

}

void output_errordump(void){

    if(error_report[0]){
        fprintf(error_dump , "In cycle %d: Write $0 Error\n", cycle);
    }
    if(error_report[1]){
        fprintf(error_dump , "In cycle %d: Overwrite HI-LO registers\n", cycle);
    }
    if(error_report[2]){
        fprintf(error_dump , "In cycle %d: Address Overflow\n", cycle);
    }
    if(error_report[3]){
        fprintf(error_dump , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(error_report[4]){
        fprintf(error_dump , "In cycle %d: Number Overflow\n", cycle);
    }
    for(int i = 0; i < 5; i++){
        error_report[i] = 0;
    }
}
