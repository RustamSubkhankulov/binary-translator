#pragma once 

#include "../list/list.h"
#include "bintrans_conf.h"
#include "../logs/errors_and_logs.h"

//===============================================

#ifdef BINTRANS_LOGS

    #define bintrans_log_report() \
            log_report()

#else 

    #define bintrans_log_report() 

#endif

//===============================================

#ifdef TRANS_STRUCT_VALID_CHECK

    #define TRANS_STRUCT_VALID(trans_struct)            \
                                                        \
    do                                                  \
    {                                                   \
                                                        \
        if (trans_struct_validator(trans_struct) == -1) \
            return -1;                                  \
                                                        \
    } while(0);

#else

    #define TRANS_STRUCT_VALID(trans_struct)

#endif 

//===============================================

#ifdef ENTITY_ADD_NAME_STR

    #define FOR_LIST_DUMP(...) __VA_ARGS__ 

#else

    #define FOR_LIST_DUMP(...)

#endif 

//===============================================

#ifndef ENTITY_ADD_NAME_STR

    #define INIT_ENTITY(trans_struct_ptr, Name)             \
                                                            \
        do                                                  \
        {                                                   \
                                                            \
            if (init_entity(trans_struct_ptr,               \
                            Name##_size, Name) == -1)       \
            {                                               \
                return -1;                                  \
            }                                               \
                                                            \
        } while(0);

#else 

    #define INIT_ENTITY(trans_struct_ptr, Name)             \
                                                            \
        do                                                  \
        {                                                   \
                                                            \
            if (init_entity(trans_struct_ptr,               \
                     Name##_size, Name, #Name) == -1)       \
            {                                               \
                return -1;                                  \
            }                                               \
                                                            \
        } while(0);

#endif 

//-----------------------------------------------

#define PATCH_ENTITY(trans_entity, pos, size, data)         \
                                                            \
    do                                                      \
    {                                                       \
                                                            \
        if (patch_entity(trans_entity, pos,                 \
                                size, data) == -1)          \
        {                                                   \
            return -1;                                      \
        }                                                   \
                                                            \
    } while (0);

//===============================================

struct Binary_input 
{

    FILE* file_ptr;
    unsigned int size;

    char* buffer;
};

//===============================================

struct Trans_entity
{
    unsigned char* data;
    unsigned int   size;

    #ifdef ENTITY_ADD_NAME_STR

        char* name_str;

    #endif 
    //type maybe?
};


//===============================================

struct Input
{
    unsigned char* buffer;
    unsigned int   pos;
    unsigned int   size;
};

//===============================================

struct Result
{
    unsigned char* buffer;
    unsigned int   size;
    uint64_t       buffer_addr;

    unsigned int   cur_pos;
};

//===============================================

struct Trans_struct
{
    struct List* entities;

    struct Input  input;
    struct Result result;

    float reg_values[16];

    #ifdef BINTRANS_LISTING

        FILE* listing;

    #endif 
};

//===============================================

struct Register_info
{
    unsigned int register_num;
    int          register_val;
};

//===============================================

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS));

int _free_binary_input     (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

int _fill_input_buffer     (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

int _input_load_to_buffer  (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _trans_sub    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_add    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_hlt    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_mul    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_div    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_push   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_pop    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_out    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_in     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_ret    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_pow    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_eq     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_mr     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_mre    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_ls     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_lse    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_neq    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_sin    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_cos    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_tg     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_ln     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_asin   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_atg    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_draw   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_jmp    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_ja     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_jae    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_jb     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_jbe    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_je     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_jne    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_call   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));


#define trans_add(struct, op_code) \
       _trans_add(struct, op_code FOR_LOGS(, LOG_ARGS)) 

#define trans_sub(struct, op_code) \
       _trans_sub(struct, op_code FOR_LOGS(, LOG_ARGS))

#define trans_hlt(struct) \
       _trans_hlt(struct FOR_LOGS(, LOG_ARGS))

#define trans_mul(struct) \
       _trans_mul(struct FOR_LOGS(, LOG_ARGS))

#define trans_div(struct) \
       _trans_div(struct FOR_LOGS(, LOG_ARGS))

#define trans_push(struct) \
       _trans_push(struct FOR_LOGS(, LOG_ARGS))

#define trans_pop(struct) \
       _trans_pop(struct FOR_LOGS(, LOG_ARGS))

#define trans_out(struct) \
       _trans_out(struct FOR_LOGS(, LOG_ARGS))

#define trans_in(struct) \
       _trans_in(struct FOR_LOGS(, LOG_ARGS))

#define trans_ret(struct) \
       _trans_ret(struct FOR_LOGS(, LOG_ARGS))

#define trans_pow(struct) \
       _trans_pow(struct FOR_LOGS(, LOG_ARGS))

#define trans_eq(struct) \
       _trans_eq(struct FOR_LOGS(, LOG_ARGS))

#define trans_mr(struct) \
       _trans_mr(struct FOR_LOGS(, LOG_ARGS))

#define trans_mre(struct) \
       _trans_mre(struct FOR_LOGS(, LOG_ARGS))

#define trans_ls(struct) \
       _trans_ls(struct FOR_LOGS(, LOG_ARGS))

#define trans_lse(struct) \
       _trans_lse(struct FOR_LOGS(, LOG_ARGS))

#define trans_neq(struct) \
       _trans_neq(struct FOR_LOGS(, LOG_ARGS))

#define trans_sin(struct) \
       _trans_sin(struct FOR_LOGS(, LOG_ARGS))

#define trans_cos(struct) \
       _trans_cos(struct FOR_LOGS(, LOG_ARGS))

#define trans_tg(struct) \
       _trans_tg(struct FOR_LOGS(, LOG_ARGS))

#define trans_ln(struct) \
       _trans_ln(struct FOR_LOGS(, LOG_ARGS))

#define trans_asin(struct) \
       _trans_asin(struct FOR_LOGS(, LOG_ARGS))

#define trans_atg(struct) \
       _trans_atg(struct FOR_LOGS(, LOG_ARGS))

#define trans_draw(struct) \
       _trans_draw(struct FOR_LOGS(, LOG_ARGS))

#define trans_jmp(struct) \
       _trans_jmp(struct FOR_LOGS(, LOG_ARGS))

#define trans_ja(struct) \
       _trans_ja(struct FOR_LOGS(, LOG_ARGS))

#define trans_jae(struct) \
       _trans_jae(struct FOR_LOGS(, LOG_ARGS))

#define trans_jb(struct) \
       _trans_jb(struct FOR_LOGS(, LOG_ARGS))

#define trans_jbe(struct) \
       _trans_jbe(struct FOR_LOGS(, LOG_ARGS))

#define trans_je(struct) \
       _trans_je(struct FOR_LOGS(, LOG_ARGS))

#define trans_jne(struct) \
       _trans_jne(struct FOR_LOGS(, LOG_ARGS))

#define trans_call(struct) \
       _trans_call(struct FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

int _trans_struct_ctor     (Trans_struct* trans_struct, Binary_input* binary_input 
                                                           FOR_LOGS(, LOG_PARAMS));

int _binary_header_check   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_translate      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_execute        (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dtor     (Trans_struct* trans_strcut FOR_LOGS(, LOG_PARAMS));

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _flush_entities_to_buf (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_translated_code  (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _count_call_buf_size   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_allocate     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_single_instruction(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_change_acc_prot(Trans_struct* trans_struct, int prot FOR_LOGS(, LOG_PARAMS));

int _init_entity(Trans_struct* trans_struct, unsigned int size, const unsigned char* data 
                            FOR_LIST_DUMP(, const char* name_str) FOR_LOGS(, LOG_PARAMS));
 
int _patch_entity(Trans_entity* trans_entity, unsigned int   patch_pos, unsigned int  patch_size,
                                              unsigned char* patch_data FOR_LOGS(, LOG_PARAMS));

int _write_listing(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _listing_message(Trans_entity* trans_entity, unsigned int res_buf_pos, 
                                    FILE* listing FOR_LOGS(, LOG_PARAMS));

#ifdef BINTRANS_LISTING

    int _init_listing_file     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

    int _end_listing_file      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

#endif 

//===============================================

#define count_call_buf_size(trans_struct) \
       _count_call_buf_size(trans_struct FOR_LOGS(, LOG_ARGS))

#define increase_entities_array(trans_struct) \
       _increase_entities_array(trans_struct FOR_LOGS(, LOG_ARGS))

#define translate_single_instruction(trans_struct) \
       _translate_single_instruction(trans_struct FOR_LOGS(, LOG_ARGS))

#define translate_instructions(trans_struct) \
       _translate_instructions(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_translated_code(trans_struct) \
       _call_translated_code(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_buf_allocate(trans_struct) \
       _call_buf_allocate(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_buf_change_acc_prot(trans_struct, prot) \
       _call_buf_change_acc_prot(trans_struct, prot FOR_LOGS(, LOG_ARGS))

#define init_listing_file(trans_struct) \
       _init_listing_file(trans_struct FOR_LOGS(, LOG_ARGS))

#define end_listing_file(trans_struct) \
       _end_listing_file(trans_struct FOR_LOGS(, LOG_ARGS))

#define write_listing(trans_struct) \
       _write_listing(trans_struct FOR_LOGS(, LOG_ARGS))

#define listing_message(trans_entity, res_buf_pos, listing) \
       _listing_message(trans_entity, res_buf_pos, listing FOR_LOGS(, LOG_ARGS))

#define binary_header_check(trans_struct) \
       _binary_header_check(trans_struct FOR_LOGS(, LOG_ARGS))

#define read_binary_input(binary_input, filename) \
       _read_binary_input(binary_input, filename FOR_LOGS(, LOG_ARGS)) 

#define free_binary_input(binary_input) \
       _free_binary_input(binary_input FOR_LOGS(, LOG_ARGS))

#define input_load_to_buffer(binary_input) \
       _input_load_to_buffer(binary_input FOR_LOGS(, LOG_ARGS))

#define fill_input_buffer(binary_input) \
       _fill_input_buffer(binary_input FOR_LOGS(, LOG_ARGS))

#define trans_struct_ctor(trans_struct, binary_input) \
       _trans_struct_ctor(trans_struct, binary_input FOR_LOGS(, LOG_ARGS))

#define trans_struct_dtor(trans_struct) \
       _trans_struct_dtor(trans_struct FOR_LOGS(, LOG_ARGS))

#define trans_struct_validator(trans_struct) \
       _trans_struct_validator(trans_struct FOR_LOGS(, LOG_ARGS))

#define binary_translate(trans_struct) \
       _binary_translate(trans_struct FOR_LOGS(, LOG_ARGS))

#define binary_execute(trans_struct) \
       _binary_execute(trans_struct FOR_LOGS(, LOG_ARGS))

#define flush_entities_to_buf(trans_struct) \
       _flush_entities_to_buf(trans_struct FOR_LOGS(, LOG_ARGS))

#define patch_entity(entity, pos, size, data) \
       _patch_entity(entity, pos, size, data FOR_LOGS(, LOG_ARGS))

#ifdef ENTITY_ADD_NAME_STR

    #define init_entity(trans_struct, size, data, name_str) \
           _init_entity(trans_struct, size, data, name_str FOR_LOGS(, LOG_ARGS))

#else

    #define init_entity(trans_struct, size, data) \
           _init_entity(trans_struct, size, data FOR_LOGS(, LOG_ARGS))

#endif 