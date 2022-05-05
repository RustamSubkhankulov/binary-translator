#pragma once 

#include "optimizer_conf.h"
#include "bintrans.h"
#include "../list/list.h"

//===============================================

struct Instr_data
{
    union
    {
        float float_value;
        int   int_value;
    };

    unsigned char  unsigned_char_value;
};

//-----------------------------------------------

struct Instr
{
    unsigned char oper_code;
    unsigned int  size;

    struct Instr_data data;

    #ifdef ADD_INSTR_NAME

        char* name_str;

    #endif 
};

//===============================================

int _binary_optimize(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _read_instructions(Input* input, List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_instructions(List* list FOR_LOGS(, LOG_PARAMS));

int _flush_instructions_to_buf(Trans_struct* trans_struct, List* list 
                                              FOR_LOGS(, LOG_PARAMS));

int _list_free_instr(List* list FOR_LOGS(, LOG_PARAMS));

//===============================================

#define list_free_instr(list) \
       _list_free_instr(list FOR_LOGS(, LOG_ARGS))

#define read_instructions(input, list) \    
       _read_instructions(input, list FOR_LOGS(, LOG_ARGS))

#define optimize_instructions(list) \
       _optimize_instructions(list FOR_LOGS(, LOG_ARGS))

#define flush_instructions_to_buf(trans_struct, list) \
       _flush_instructions_to_buf(trans_struct, list FOR_LOGS(, LOG_ARGS))

#define binary_optimize(trans_struct) \
       _binary_optimize(trans_struct FOR_LOGS(, LOG_ARGS))

       