#pragma once 

//===============================================

#include "bintrans.h"
#include "instr.h"
#include "patch.h"

//===============================================

#ifndef ENTITY_ADD_NAME_STR

    #define INIT_ENTITY(trans_struct_ptr, Name)             \
                                                            \
        do                                                  \
        {                                                   \
                                                            \
            if (init_entity(trans_struct_ptr,               \
                Name##_size, Name, Name##_type) == -1)      \
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
              Name##_size, Name, Name##_type, #Name) == -1) \
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

#define INIT_SAVE_XMM_0_1_13(trans_struct)                  \
                                                            \
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);              \
    INIT_ENTITY(trans_struct, Movd_r14d_xmm1);              \
    INIT_ENTITY(trans_struct, Movd_r13d_xmm13);          


#define INIT_RESTORE_XMM_0_1_13(trans_struct)               \
                                                            \
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);              \
    INIT_ENTITY(trans_struct, Movd_xmm1_r14d);              \
    INIT_ENTITY(trans_struct, Movd_xmm13_r13d);         

//-----------------------------------------------

#define INIT_SAVE_XMM_0_13(trans_struct)                    \
                                                            \
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);              \
    INIT_ENTITY(trans_struct, Movd_r13d_xmm13);     

#define INIT_RESTORE_XMM_0_13(trans_struct)                 \
                                                            \
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);              \
    INIT_ENTITY(trans_struct, Movd_xmm13_r13d);

//-----------------------------------------------

#define INIT_SAVE_XMM_0_1(trans_struct)                     \
                                                            \
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);              \
    INIT_ENTITY(trans_struct, Movd_r14d_xmm1);

#define INIT_RESTORE_XMM_0_1(trans_struct)                  \
                                                            \
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);              \
    INIT_ENTITY(trans_struct, Movd_xmm1_r14d);

//===============================================

int _trans_arithm (Trans_struct* trans_struct, 
                   unsigned char oper_code FOR_LOGS(, LOG_PARAMS));

int _trans_hlt    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_push   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_pop    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_out    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_in     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_ret    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_pow    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_draw   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_call   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_cond_jmp(Trans_struct* trans_struct, unsigned char op_code
                                             FOR_LOGS(, LOG_PARAMS));

int _trans_jmp(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_compare(Trans_struct* trans_struct, unsigned char op_code 
                                             FOR_LOGS(, LOG_PARAMS));

int _trans_std_func(Trans_struct* trans_struct, unsigned char op_code
                                             FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _patch_short_cond_jump(Trans_struct* trans_struct, unsigned char patch_byte 
                                                        FOR_LOGS(, LOG_PARAMS));

int _patch_near_cond_jump(Trans_struct* trans_struct, unsigned int patch_bytes 
                                                      FOR_LOGS(, LOG_PARAMS));

unsigned int _get_unsigned_int_from_input(Trans_struct* trans_struct 
                                             FOR_LOGS(, LOG_PARAMS));

unsigned char _get_unsigned_char_from_input(Trans_struct* trans_struct 
                                               FOR_LOGS(, LOG_PARAMS));

float _get_float_from_input   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _init_and_patch_cvtss2si  (Trans_struct* trans_sturct FOR_LOGS(, LOG_PARAMS));

int _init_and_patch_push_xmmi (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _init_and_patch_pop_xmmi  (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _init_and_patch_mov_r13d_0(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));


//===============================================


#define init_and_patch_cvtss2si(trans_struct) \
       _init_and_patch_cvtss2si(trans_struct FOR_LOGS(, LOG_ARGS))

#define init_and_patch_push_xmmi(trans_struct) \
       _init_and_patch_push_xmmi(trans_struct FOR_LOGS(, LOG_ARGS))

#define init_and_patch_pop_xmmi(trans_struct) \
       _init_and_patch_pop_xmmi(trans_struct FOR_LOGS(, LOG_ARGS))

#define init_and_patch_mov_r13d_0(trans_struct) \
       _init_and_patch_mov_r13d_0(trans_struct FOR_LOGS(, LOG_ARGS))

#define get_float_from_input(trans_struct) \
       _get_float_from_input(trans_struct FOR_LOGS(, LOG_ARGS))

#define get_unsigned_char_from_input(trans_struct) \
       _get_unsigned_char_from_input(trans_struct FOR_LOGS(, LOG_ARGS))

#define get_unsigned_int_from_input(trans_struct) \
       _get_unsigned_int_from_input(trans_struct FOR_LOGS(, LOG_ARGS))

#define patch_short_cond_jump(trans_struct, patch_byte) \
       _patch_short_cond_jump(trans_struct, patch_byte FOR_LOGS(, LOG_ARGS))

#define patch_near_cond_jump(trans_struct, patch_bytes) \
       _patch_near_cond_jump(trans_struct, patch_bytes FOR_LOGS(, LOG_ARGS))

#define trans_arithm(trans_struct, oper_code) \
       _trans_arithm(trans_struct, oper_code FOR_LOGS(, LOG_ARGS))

#define trans_hlt(struct) \
       _trans_hlt(struct FOR_LOGS(, LOG_ARGS))

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

// #define trans_eq_neq(struct, op_code) 
//        _trans_eq_neq(struct, op_code FOR_LOGS(, LOG_ARGS))

#define trans_compare(struct, op_code) \
       _trans_compare(struct, op_code FOR_LOGS(, LOG_ARGS))

#define trans_std_func(struct, op_code) \
       _trans_std_func(struct, op_code FOR_LOGS(, LOG_ARGS))

#define trans_draw(struct) \
       _trans_draw(struct FOR_LOGS(, LOG_ARGS))

#define trans_jmp(struct) \
       _trans_jmp(struct FOR_LOGS(, LOG_ARGS))

#define trans_cond_jmp(struct, op_code) \
       _trans_cond_jmp(struct, op_code FOR_LOGS(, LOG_ARGS))

#define trans_call(struct) \
       _trans_call(struct FOR_LOGS(, LOG_ARGS))

