#include <assert.h>

#include "processor.h"

int main(int argc, char* argv[]) {

    int err_val = 0;

    err_val = proc_check_command_line_args(argc); $

    FILE* logs_file = open_log_file(argv[4]);
    if (logs_file == NULL)
        return -1;

    proc_smpl_log_report();

    struct Procstruct procstruct = { 0 };

    FILE* fp = open_code_file(argv[1], "rb");
    if (fp == NULL)
        return -1;

    err_val = init_procstruct(&procstruct, fp); $

    err_val = close_code_file(fp); $

    err_val = open_proc_input(argv[2]); $
    err_val = open_proc_output(argv[3]); $

    err_val = proc_perform(&procstruct); $

    err_val = dtor_procstruct(&procstruct); $

    err_val = close_proc_input(); $
    err_val = close_proc_output(); $

    err_val = close_log_file(); $ 
}