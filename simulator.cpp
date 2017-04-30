//#include "simulator.h"
//#include "five_stages.h"

#include "simulator.h"


void simulator(void){

    //IF = imemory[pc_IF / 4];
    //IF_ID = IF;
    //pc_ID = 4;
    while(quit_flag < 5){

        pc_IF = pc_ID;
        output_snapshot_reg();
        quit_flag = 0;


        write_back();
        data_memory();
        excution();
        instruction_decoder();
        instruction_fetch();

        cycle++;
        if(cycle == 21){
            printf("0x%08X\n", IF);
            printf("0x%08X\n", ID);
            printf("0x%08X\n", EX);
            printf("0x%08X\n", DM);
            printf("0x%08X\n", WB);

        }

        output_snapshot_ins();
        output_errordump();



        if(pc_ID != pc_IF){
            pc_changed = 1;
        }else{
            pc_changed = 0;
        }
        //pc_IF = pc_ID;
    }

}
