#pragma once 

#include "patch_conf.h"
#include "../logs/errors_and_logs.h"
#include "bintrans.h"

//===============================================

#define ADD_PATCH_CONST(trans_struct, const_val)              \
                                                               \
    if (add_patch_instr_const(trans_struct, const_val) == -1)   \
        return -1;

#define ADD_PATCH_STD_FUNC(trans_struct, code)                \
                                                               \
    if (add_patch_instr_std_func(trans_struct, code) == -1)     \
        return -1;

#define ADD_PATCH_RAM_START_ADDR(trans_struct)                 \
                                                                \
    if (add_patch_instr_ram_start_addr(trans_struct) == -1)      \
        return -1;

//===============================================

int _binary_patch_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _init_patch_struct(Patch* patch_struct FOR_LOGS(, LOG_PARAMS));

int _free_patch_struct(Patch* patch_struct FOR_LOGS(, LOG_PARAMS));

int _patch_instructions_resize(Patch* patch_struct FOR_LOGS(, LOG_PARAMS));

int _patch_instruction(Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _add_patch_instr_ram_start_addr(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _add_patch_instr_std_func(Trans_struct* trans_struct, int std_func_code FOR_LOGS(, LOG_PARAMS));

int _add_patch_instr_const(Trans_struct* trans_struct, float const_value FOR_LOGS(, LOG_PARAMS));

Patch_instr* _patch_instr_init(Trans_struct* trans_struct, int type FOR_LOGS(, LOG_PARAMS));

int _patch_const         (Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS));

int _patch_std_func      (Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS));

int _patch_ram_start_addr(Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS));

float* _check_const_added(Patch* patch, float* consts_buffer, float value 
                                                  FOR_LOGS(, LOG_PARAMS));

float* _add_const_to_buffer(Trans_struct* trans_struct, float value   
                                             FOR_LOGS(, LOG_PARAMS));

int _fix_up_jumps(Jumps* jumps FOR_LOGS(, LOG_PARAMS));

//===============================================

#define fix_up_jumps(jumps) \
       _fix_up_jumps(jumps FOR_LOGS(, LOG_ARGS))

#define add_const_to_buffer(trans_struct, value) \
       _add_const_to_buffer(trans_struct, value FOR_LOGS(, LOG_ARGS))

#define check_const_added(patch, buffer, val) \
       _check_const_added(patch, buffer, val FOR_LOGS(, LOG_ARGS))

#define patch_instr_init(trans_struct, type) \
       _patch_instr_init(trans_struct, type FOR_LOGS(, LOG_ARGS))

#define patch_const(trans_struct, patch_instr) \
       _patch_const(trans_struct, patch_instr FOR_LOGS(, LOG_ARGS))

#define patch_std_func(trans_struct, patch_instr) \
       _patch_std_func(trans_struct, patch_instr FOR_LOGS(, LOG_ARGS))

#define patch_ram_start_addr(trans_struct, patch_instr) \
       _patch_ram_start_addr(trans_struct, patch_instr FOR_LOGS(, LOG_ARGS))
       
#define patch_instruction(trans_struct, patch_instr) \
       _patch_instruction(trans_struct, patch_instr FOR_LOGS(, LOG_ARGS))

#define binary_patch_instructions(trans_struct) \
       _binary_patch_instructions(trans_struct FOR_LOGS(, LOG_ARGS))

#define init_patch_struct(patch_struct) \
       _init_patch_struct(patch_struct FOR_LOGS(, LOG_ARGS))

#define free_patch_struct(patch_struct) \
       _free_patch_struct(patch_struct FOR_LOGS(, LOG_ARGS))

#define patch_instructions_resize(patch_struct) \
       _patch_instructions_resize(patch_struct FOR_LOGS(, LOG_ARGS))

#define add_patch_instr_const(trans_struct, const_val) \
       _add_patch_instr_const(trans_struct, const_val FOR_LOGS(, LOG_ARGS))

#define add_patch_instr_std_func(trans_struct, std_func_code) \
       _add_patch_instr_std_func(trans_struct, std_func_code FOR_LOGS(, LOG_ARGS))

#define add_patch_instr_ram_start_addr(trans_struct) \
       _add_patch_instr_ram_start_addr(trans_struct FOR_LOGS(, LOG_ARGS))