#pragma once

#include "../cpu/processor.h"
#include "../assembler/processor_general.h"
#include "../assembler/config.h"

struct Disasmstruct {

    long code_file_size;
    const unsigned char* code_array;
    const unsigned char* ip;

    struct Header header;
};

//===================================================================

#define fprintf_int_argument                                          \
    do                                                                \
    {                                                                 \
        err = fprintf(disasm_output, "%d",                            \
                      *(int*)disasmstruct->ip);                       \
                                                                      \
        if (err < 0)                                                  \
            error_report(FWRITE_ERR);                                 \
                                                                      \
        disasmstruct->ip += sizeof(int);                              \
        DISASMSTRUCT_IP_CHECK(disasmstruct);                          \
                                                                      \
    } while(0);

//===================================================================

#define fprintf_register_argument                                     \
do                                                                    \
{                                                                     \
    err = fprintf(disasm_output, "%cx",                               \
                 *disasmstruct->ip + Ascii_offset);                   \
                                                                      \
    if (err < 0)                                                      \
        error_report(FWRITE_ERR);                                     \
                                                                      \
    disasmstruct->ip++;                                               \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                              \
                                                                      \
} while(0);

//===================================================================

#define fprintf_elem_t_argument                                       \
do                                                                    \
{                                                                     \
    err = fprintf(disasm_output, ELEM_SPEC_2 ,                        \
                  *(elem_t*)disasmstruct->ip);                        \
                                                                      \
    if (err < 0)                                                      \
        error_report(FWRITE_ERR);                                     \
                                                                      \
    disasmstruct->ip += sizeof(elem_t);                               \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                              \
                                                                      \
} while(0);

//===================================================================

#define fprintf_var_arguments                                         \
do                                                                    \
{                                                                     \
    err = fprintf(disasm_output, "%c ",                               \
                 *disasmstruct->ip  + Ascii_offset);                  \
                                                                      \
    if (err < 0)                                                      \
        error_report(FWRITE_ERR);                                     \
                                                                      \
    disasmstruct->ip++;                                               \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                              \
                                                                      \
    elem_t value = *(elem_t*)disasmstruct->ip;                        \
                                                                      \
    disasmstruct->ip += sizeof(elem_t);                               \
    DISASMSTRUCT_IP_CHECK(disasmstruct);                              \
                                                                      \
    char sign = '+';                                                  \
                                                                      \
    if (value < 0)                                                    \
        sign = '-';                                                   \
                                                                      \
    err = fprintf(disasm_output, "%c " ELEM_SPEC_2 ,                  \
                  sign, *(elem_t*)disasmstruct->ip);                  \
                                                                      \
    if (err < 0)                                                      \
        error_report(FWRITE_ERR);                                     \
                                                                      \
} while(0);

//===================================================================

#define DISASMSTRUCT_IP_CHECK(disasmstruct) {                         \
                                                                      \
    if (disasmstruct == NULL)                                         \
                                                                      \
        error_report(INV_DISASMSTRUCT_PTR);                           \
}

//===================================================================

#define DISASMSTRUCT_IP_CHECK(disasmstruct) {                         \
                                                                      \
    if (disasmstruct->ip - disasmstruct->code_array                   \
       > disasmstruct->code_file_size)                                \
                                                                      \
        error_report(INV_INSTR_PTR)                                   \
}

//===================================================================

#define init_disasmstruct(disasmstruct, fp) \
       _init_disasmstruct(disasmstruct, fp, LOG_ARGS)

#define disassemble_code(disasmstruct, fp) \
       _disassemble_code(disasmstruct, fp, LOG_ARGS) 

#define dtor_disasmstruct(disasmstruct) \
       _dtor_disasmstruct(disasmstruct, LOG_ARGS) 

#define disasm_header_check(disasmstruct) \
       _disasm_header_check(disasmstruct, LOG_ARGS)
       
#define disassemble_command(disasmstruct, fp) \
       _disassemble_command(disasmstruct, fp, LOG_ARGS)   

//===================================================================

#define open_file(filename, mode, err) \
       _open_file(filename, mode, err, LOG_ARGS) 

#define close_file(fp) \
       _close_file(fp ,LOG_ARGS)

#define open_disasm_listing_file() \
       _opem_disasm_listing_file(LOG_ARGS)

#define close_disasm_listing_file() \
       _close_disasm_listing_file(LOG_ARGS) 

//===================================================================

int _init_disasmstruct(struct Disasmstruct* disasmstruct, FILE* fp,
                                                LOG_PARAMS);

int _disasm_header_check(struct Disasmstruct* disasmstruct, LOG_PARAMS);

int _disassemble_command(struct Disasmstruct* disasmstruct, FILE* disasm_output,
                                                             LOG_PARAMS);

int _disassemble_code(struct Disasmstruct* disasmstruct, FILE* disasm_output,
                                                          LOG_PARAMS);

int _dtor_disasmstruct(struct Disasmstruct* disasmstruct, LOG_PARAMS);

//===================================================================

FILE* _open_file(const char* filename, const char* mode,  int* err,  
                                                       LOG_PARAMS);


int _close_file(FILE* fp, LOG_PARAMS);

int _open_disasm_listing_file(LOG_PARAMS);

int _close_disasm_listing_file(LOG_PARAMS);