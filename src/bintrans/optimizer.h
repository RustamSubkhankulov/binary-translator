#pragma once 

#include "optimizer_conf.h"
#include "bintrans.h"
#include "../list/list.h"

//===============================================

struct Dynamic_array
{
    unsigned int* inp_dst;
    Instr** instr; 

    unsigned int num;
    unsigned int cap;
};

//-----------------------------------------------

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

    unsigned int inp_pos;

    #ifdef ADD_INSTR_NAME

        char* name_str;

    #endif 
};

//-----------------------------------------------

static const unsigned char Label_oper_code = 0xFF;

//===============================================

int _binary_optimize(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _read_instructions(Input* input, List* list, Dynamic_array* inp_dst FOR_LOGS(, LOG_PARAMS));

int _optimize_instructions(List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_consts  (List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_arithm  (List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_std_func(List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_mul_zero(List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_add_sub_zero(List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_mul_one (List* list FOR_LOGS(, LOG_PARAMS));

int _fold_std_func(List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS));

int _fold_arithm  (List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS));

int _fold_mul_zero(List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS));

int _fold_add_sub_zero(List* list, int zero_index, int oper_index 
                                                   FOR_LOGS(, LOG_PARAMS));

int _fold_mul_one(List* list, int one_index, int mul_index 
                                             FOR_LOGS(, LOG_PARAMS));

int _count_instructions_size(List* list FOR_LOGS(, LOG_PARAMS));

int _flush_instructions_to_buf(Trans_struct* trans_struct, List* list 
                                              FOR_LOGS(, LOG_PARAMS));

int _change_and_store_header(unsigned char* new_buf, unsigned char* old_buf, 
                             unsigned int   new_size FOR_LOGS(, LOG_PARAMS));

int _list_free_instr(List* list FOR_LOGS(, LOG_PARAMS));

Instr* _init_instr(unsigned char oper_code, Input* input, Dynamic_array* jump_dst 
                                                          FOR_LOGS(, LOG_PARAMS));

#ifdef ADD_INSTR_NAME

    int _add_instruction_name(Instr* instr FOR_LOGS(, LOG_PARAMS));

#endif 

//-----------------------------------------------

int _load_float_value        (Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_unsigned_char_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_int_value          (Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_instr_data         (unsigned char oper_code, Input* input, 
                              Instr* instr, Dynamic_array* jump_dst FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _store_int_value          (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_float_value        (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_unsigned_char_value(Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_instr_data         (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _dynamic_array_ctor(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS));

int _dynamic_array_dtor(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS));

int _dynamic_array_increase(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS));

int _dynamic_array_add(Dynamic_array* dynamic_array, unsigned int value, Instr* instr 
                                                              FOR_LOGS(, LOG_PARAMS));
                                                              
//-----------------------------------------------

int _insert_labels_in_list(List* list, Dynamic_array* jump_dst FOR_LOGS(, LOG_PARAMS));

Instr* _init_label(int inp_dst FOR_LOGS(, LOG_PARAMS));

int _recalculate_jump_dst(List* list, Dynamic_array* jump_dst FOR_LOGS(, LOG_PARAMS));

int _replace_jump_dst(Dynamic_array* jump_dst, int old_jump_dst, 
                                               int new_jump_dst FOR_LOGS(, LOG_PARAMS));

//===============================================

#define replace_jump_dst(jump_dst, old_dst, new_dst) \
       _replace_jump_dst(jump_dst, old_dst, new_dst FOR_LOGS(, LOG_ARGS))

#define recalculate_jump_dst(list, jump_dst) \
       _recalculate_jump_dst(list, jump_dst FOR_LOGS(, LOG_ARGS))

#define init_label(inp_dst) \
       _init_label(inp_dst FOR_LOGS(, LOG_ARGS))

#define insert_labels_in_list(list, jump_dst) \
       _insert_labels_in_list(list, jump_dst FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define dynamic_array_ctor(dynamic_array) \
       _dynamic_array_ctor(dynamic_array FOR_LOGS(, LOG_ARGS))

#define dynamic_array_dtor(dynamic_array) \
       _dynamic_array_dtor(dynamic_array FOR_LOGS(, LOG_ARGS))

#define dynamic_array_increase(dynamic_array) \
       _dynamic_array_increase(dynamic_array FOR_LOGS(, LOG_ARGS))

#define dynamic_array_add(dynamic_array, inp_dst, instr) \
       _dynamic_array_add(dynamic_array, inp_dst, instr FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define store_int_value(instr, buf, buf_pos) \
       _store_int_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_float_value(instr, buf, buf_pos) \
       _store_float_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_unsigned_char_value(instr, buf, buf_pos) \
       _store_unsigned_char_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_instr_data(instr, buf, buf_pos) \
       _store_instr_data(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define load_float_value(input, instr) \
       _load_float_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_int_value(input, instr) \
       _load_int_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_unsigned_char_value(input, instr) \
       _load_unsigned_char_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_instr_data(oper_code, input, instr, jump_dst) \
       _load_instr_data(oper_code, input, instr, jump_dst FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define init_instr(oper_code, input, jump_dst) \
       _init_instr(oper_code, input, jump_dst FOR_LOGS(, LOG_ARGS))

#define count_instructions_size(list) \
       _count_instructions_size(list FOR_LOGS(, LOG_ARGS))

#define list_free_instr(list) \
       _list_free_instr(list FOR_LOGS(, LOG_ARGS))

#define read_instructions(input, list, inp_dst) \
       _read_instructions(input, list, inp_dst FOR_LOGS(, LOG_ARGS))

#define optimize_instructions(list) \
       _optimize_instructions(list FOR_LOGS(, LOG_ARGS))

#define optimize_consts(list) \
       _optimize_consts(list FOR_LOGS(, LOG_ARGS))

#define optimize_arithm(list) \
       _optimize_arithm(list FOR_LOGS(, LOG_ARGS))

#define optimize_std_func(list) \
       _optimize_std_func(list FOR_LOGS(, LOG_ARGS))

#define optimize_mul_zero(list) \
       _optimize_mul_zero(list FOR_LOGS(, LOG_ARGS))

#define optimize_mul_one(list) \
       _optimize_mul_one(list FOR_LOGS(, LOG_ARGS))

#define fold_std_func(list, cur, nxt) \
       _fold_std_func(list, cur, nxt FOR_LOGS(, LOG_ARGS))

#define fold_arithm(list, cur, nxt) \
       _fold_arithm(list, cur, nxt FOR_LOGS(, LOG_ARGS))

#define fold_mul_zero(list, cur, nxt) \
       _fold_mul_zero(list, cur, nxt FOR_LOGS(, LOG_ARGS))

#define fold_add_sub_zero(list, zero, oper) \
       _fold_add_sub_zero(list, zero, oper FOR_LOGS(, LOG_ARGS))

#define fold_mul_one(list, zero, oper) \
       _fold_mul_one(list, zero, oper FOR_LOGS(, LOG_ARGS))

#define optimize_add_sub_zero(list) \
       _optimize_add_sub_zero(list FOR_LOGS(, LOG_ARGS))

#define change_and_store_header(new_buf, old_buf, size) \
       _change_and_store_header(new_buf, old_buf, size FOR_LOGS(, LOG_ARGS))

#define flush_instructions_to_buf(trans_struct, list) \
       _flush_instructions_to_buf(trans_struct, list FOR_LOGS(, LOG_ARGS))

#define binary_optimize(trans_struct) \
       _binary_optimize(trans_struct FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#ifdef ADD_INSTR_NAME

    #define add_instruction_name(instr) \
           _add_instruction_name(instr FOR_LOGS(, LOG_ARGS))

#endif 

       