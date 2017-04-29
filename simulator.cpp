//#include "simulator.h"
//#include "five_stages.h"

#include "simulator.h"


void simulator(void){

    while(quit_flag < 5){
        quit_flag = 0;
        write_back();
        data_memory();
        excution();
        instruction_decoder();
        instruction_fetch();
        output_errordump();
        output_snapshot();
        cycle++;
        pc_IF = pc_ID;
    }

}
