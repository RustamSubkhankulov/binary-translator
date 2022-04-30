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

#define LAST_ENTITY trans_struct->entities->data[trans_struct->entities->tail] 

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

    int type;

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

enum Patch_types
{
    RAM_START_ADDR = 228,
    CONST          = 229,
    STD_FUNC       = 230,
};

//-----------------------------------------------

union Info
{
    float const_value;
    int   std_func_code;
};

//-----------------------------------------------

struct Patch_instr
{
    int patch_type;

    unsigned int res_buf_pos;

    Trans_entity* entity;
    Info info;
};

//-----------------------------------------------

struct Patch
{
    unsigned int num;
    unsigned int cap;

    Patch_instr* instructions;

    unsigned int  num_of_consts; 

    unsigned char ram_is_used;
    float*        ram_buffer;
};

//===============================================

struct Jumps
{
    unsigned int* inp_dst;

    unsigned int* res_dst;
    unsigned int* res_pos;

    Trans_struct** entities;

    unsigned int num;
    unsigned int cap;
};

//===============================================

struct Trans_struct
{
    struct List* entities;

    struct Input  input;
    struct Result result;

    struct Patch patch;
    struct Jumps jumps;

    float* ram_buffer;

    float* consts_buffer;
    unsigned int consts_ct;

    //float reg_values[16];

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

int _trans_struct_ctor     (Trans_struct* trans_struct, Binary_input* binary_input 
                                                           FOR_LOGS(, LOG_PARAMS));

int _binary_header_check   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_translate      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_execute        (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dtor     (Trans_struct* trans_strcut FOR_LOGS(, LOG_PARAMS));

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _flush_entities_to_buf (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_translated_code  (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_prepare      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _count_call_buf_size   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_allocate     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_single_instruction(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_change_acc_prot(Trans_struct* trans_struct, int prot FOR_LOGS(, LOG_PARAMS));

int _init_entity(Trans_struct* trans_struct, unsigned int size, const unsigned char* data, int type
                                      FOR_LIST_DUMP(, const char* name_str) FOR_LOGS(, LOG_PARAMS));
 
int _patch_entity(Trans_entity* trans_entity, unsigned int   patch_pos, unsigned int  patch_size,
                                              unsigned char* patch_data FOR_LOGS(, LOG_PARAMS));

int _write_listing(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _listing_message(Trans_entity* trans_entity, unsigned int res_buf_pos, 
                                    FILE* listing FOR_LOGS(, LOG_PARAMS));

int _consts_buffer_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _ram_buffer_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

#ifdef BINTRANS_LISTING

    int _init_listing_file     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

    int _end_listing_file      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

#endif 

//===============================================

#define ram_buffer_allocate(trans_struct) \
       _ram_buffer_allocate(trans_struct FOR_LOGS(, LOG_ARGS))

#define consts_buffer_allocate(trans_struct) \
       _consts_buffer_allocate(trans_struct FOR_LOGS(, LOG_ARGS))

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

#define call_buf_prepare(trans_struct) \
       _call_buf_prepare(trans_struct FOR_LOGS(, LOG_ARGS))

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

    #define init_entity(trans_struct, size, data, type, name_str) \
           _init_entity(trans_struct, size, data, type, name_str FOR_LOGS(, LOG_ARGS))

#else

    #define init_entity(trans_struct, size, data, type) \
           _init_entity(trans_struct, size, data, type FOR_LOGS(, LOG_ARGS))

#endif 