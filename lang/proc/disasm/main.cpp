#include "disasm.h"

int main(int argc, char* argv[]) {

    FILE* logs_file = open_log_file(argv[3]);

    disasm_smpl_log_report();
    int err_val = 0;

    struct Disasmstruct disasmstruct = { 0 };

    FILE* fp = open_code_file(argv[1], "rb");
    if (fp == NULL)
        return -1;

    err_val = init_disasmstruct(&disasmstruct, fp); $
    err_val = close_code_file(fp); $

    FILE* disasm_out = open_file(argv[2], "wb", &err_val); $

    err_val = disassemble_code(&disasmstruct, disasm_out); $

    err_val = close_file(disasm_out); $

    err_val = dtor_disasmstruct(&disasmstruct);
    close_log_file();
    
}