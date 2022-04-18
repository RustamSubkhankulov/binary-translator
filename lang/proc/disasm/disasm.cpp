#include <string.h>
#include <stdlib.h>

#include "disasm.h"

//===================================================================

static FILE* Disasm_listing_file = NULL;

//===================================================================


int _init_disasmstruct(struct Disasmstruct* disasmstruct, FILE* fp,
                                                     LOG_PARAMS) {

    disasm_log_report();

    FILE_PTR_CHECK(fp);

    DISASMSTRUCT_IP_CHECK(disasmstruct);

    long code_file_size = 0;

    const unsigned char* code_array = read_from_file(fp, &code_file_size);
    CHAR_PTR_CHECK(code_array);
    
    disasmstruct->code_array = code_array;
    disasmstruct->code_file_size = code_file_size;

    int header_check_ret = disasm_header_check(disasmstruct);
    if (header_check_ret == -1)
        return -1;

    disasmstruct->ip = disasmstruct->code_array + sizeof(struct Header);

    return 0;
}

//===================================================================

int _disasm_header_check(struct Disasmstruct* disasmstruct, LOG_PARAMS) {

    disasm_log_report();

    DISASMSTRUCT_IP_CHECK(disasmstruct);

    memcpy(&disasmstruct->header, 
            disasmstruct->code_array, 
            sizeof(struct Header));

    if (disasmstruct->header.signature != SIGN)
        error_report(HDR_INV_SIGN);

    if (disasmstruct->header.version != VERSION)
        error_report(HDR_INV_VERSION);

    if (disasmstruct->header.file_size != disasmstruct->code_file_size)
        error_report(HDR_INV_FILE_SIZE);;

    return 0;
}

//===================================================================

int _disassemble_code(Disasmstruct* disasmstruct, FILE* disasm_output,
                                                           LOG_PARAMS) {

    disasm_log_report();
    DISASMSTRUCT_IP_CHECK(disasmstruct);
    FILE_PTR_CHECK(disasm_output);

    while(disasmstruct->ip - disasmstruct->code_array
                           < disasmstruct->code_file_size) {

            int ret_val = disassemble_command(disasmstruct, 
                                             disasm_output);
            if (ret_val == -1)
                return -1;
        }

    return 0;
}

//===================================================================

#define DEF_JMP_(num, name, code, hash, instructions)               \
                                                                    \
case code: {                                                        \
                                                                    \
    int err = fprintf(disasm_output, "%s ", #name);                 \
    if (err < 0)                                                    \
        error_report(FWRITE_ERR);                            \
                                                                    \
    disasmstruct->ip++;                                             \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                            \
                                                                    \
    fprintf_int_argument;                                           \
    fprintf(disasm_output, "\n");                                   \
                                                                    \
    break;                                                          \
}

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions)               \
                                                                    \
case code: {                                                        \
                                                                    \
    int err = fprintf(disasm_output, "%s ", #name);                 \
    if (err < 0)                                                    \
        error_report(FWRITE_ERR);                            \
                                                                    \
    disasmstruct->ip++;                                             \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                           \
                                                                    \
    if (num != 0) {                                                 \
                                                                    \
        char is_ram_using = 0;                                      \
                                                                    \
        if ((oper_code & ~RAM_MASK) == RAM_MASK) {                  \
                                                                    \
            fprintf(disasm_output, "[");                            \
            is_ram_using = 1;                                       \
        }                                                           \
                                                                    \
        for (int ct = 0; ct < num; ct++) {                          \
                                                                    \
            switch(oper_code & ~OPER_CODE_MASK ) {                  \
                                                                    \
                case IMM_MASK:                                      \
                    fprintf_elem_t_argument;                        \
                    break;                                          \
                                                                    \
                case REGISTER_MASK :                                \
                    fprintf_register_argument;                      \
                    break;                                          \
                                                                    \
                case REGISTER_MASK | IMM_MASK :                     \
                    fprintf_var_arguments;                          \
                    break;                                          \
            }                                                       \
        }                                                           \
                                                                    \
        if (is_ram_using)                                           \
            fprintf(disasm_output, "]");                            \
    }                                                               \
                                                                    \
    fprintf(disasm_output, "\n");                                   \
    break;                                                          \
}                                                                

//===================================================================

int _disassemble_command(struct Disasmstruct* disasmstruct, 
                                        FILE* disasm_output, LOG_PARAMS) {

    disasm_log_report();

    DISASMSTRUCT_IP_CHECK(disasmstruct);
    DISASMSTRUCT_IP_CHECK(disasmstruct);

    unsigned char oper_code = *(disasmstruct->ip);

    switch(oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"
        #include "../text_files/jumps.txt"

        default: 
            error_report(DISASM_INV_OPER_CODE);
            break;
    }

    #undef DEF_CMD_
    #undef DEF_JMP_

    return 0;
}

//===================================================================

int _dtor_disasmstruct(struct Disasmstruct* disasmstruct, LOG_PARAMS) {

    disasm_log_report();

    DISASMSTRUCT_IP_CHECK(disasmstruct);
    DISASMSTRUCT_IP_CHECK(disasmstruct);

    if (disasmstruct->code_array == NULL)
        error_report(INV_CODE_ARRAY_PTR);

    free((void*)disasmstruct->code_array);

    return 0;
}

//===================================================================

int _open_disasm_listing_file(const char* filename, LOG_PARAMS) {

    disasm_log_report();
    extern FILE* Disasm_listing_file;

    FILE* fp = fopen(filename, "wb");

    if (fp == NULL)
        error_report(FOPEN_ERROR);

    Disasm_listing_file = fp;

    return 0;
}

//===================================================================

int _close_disasm_listing_file(LOG_PARAMS) {

    disasm_log_report();
    extern FILE* Disasm_listing_file;

    int fclose_ret = fclose(Disasm_listing_file);

    if (fclose_ret == EOF)
        error_report(FCLOSE_ERR);

    return 0;
}

//===================================================================

FILE* _open_file(const char* filename, const char* mode, int* err,  
                                                       LOG_PARAMS) {

    disasm_log_report();

    if (filename == NULL) {

        set_global_error_code(INV_FILE_NAME);
        global_error_process();

        *err = -1;
        return NULL;
    }

    if (mode == NULL) {

        set_global_error_code(FOPEN_ERROR);
        global_error_process();

        *err = -1;
        return NULL;
    }

    FILE* fp = fopen(filename, mode);
    if (fp == NULL) {

        set_global_error_code(FOPEN_ERROR);
        global_error_process();

        *err = -1;
        return NULL;
    }

    return fp;
}

//===================================================================

int _close_file(FILE* fp, LOG_PARAMS) {

    if (fp != stdin) {

        int fclose_ret = fclose(fp);
        if (fclose_ret == EOF) 
            error_report(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================