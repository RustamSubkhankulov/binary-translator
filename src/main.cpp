#include <stdlib.h>

#include "global_conf.h"
#include "bintrans/bintrans.h"
#include "bintrans/patch.h"
#include "bintrans/optimizer.h"

//===============================================

int main(int argc, const char* argv[])
{

    #ifdef LOGS
        FILE* logs_file = open_log_file(Logfile_name);
        if (!logs_file)
            return -1;
    #endif 

    //-------------------------------------------

    int ret_val = 0;

    // Read cmndline options

    struct Cmndline cmndline = { 0 };

    ret_val = read_cmndline(&cmndline, argc, argv);
    if (ret_val == -1);

    //-------------------------------------------

    // Prologue

    Binary_input binary_input = { 0 };

    ret_val = read_binary_input(&binary_input, cmndline.input_name);
    if (ret_val == -1) return -1;

    Trans_struct trans_struct = { 0 };

    ret_val = trans_struct_ctor(&trans_struct, &binary_input);
    if (ret_val == -1) return -1;

    //-------------------------------------------

    // Main actions

    if (cmndline.opt)
    {
        ret_val = binary_optimize(&trans_struct);
        if (ret_val == -1) return -1;
    } 

    ret_val = binary_translate(&trans_struct);
    if (ret_val == -1) return -1;

    ret_val = binary_patch_instructions(&trans_struct);
    if (ret_val == -1) return -1;

    ret_val = binary_execute(&trans_struct);
    if (ret_val == -1) return -1;

    //-------------------------------------------

    //Epilogue

    ret_val = free_binary_input(&binary_input);
    if (ret_val == -1) return -1;

    ret_val = trans_struct_dtor(&trans_struct);
    if (ret_val == -1) return -1;

    //-------------------------------------------


    #ifdef LOGS
        close_log_file();
    #endif 

    return 0;
}