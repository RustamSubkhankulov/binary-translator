#include <stdlib.h>

//===============================================

#include "trans.h"
#include "../general/general.h"
#include "standard.h"
#include "../../lang/proc/assembler/processor_general.h"

//===============================================

int _trans_arithm(Trans_struct* trans_struct, 
                  unsigned char oper_code FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // Save xmm15 value in r15d
    INIT_ENTITY(trans_struct, Movd_r15d_xmm15);

    // Get first value
    INIT_ENTITY(trans_struct, Movss_xmm15_dword_rsp_plus_8);

    switch(oper_code)
    {
        case ADD:
        {
            INIT_ENTITY(trans_struct, Addss_xmm15_dword_rsp);
            break;        
        }

        case SUB:
        {
            INIT_ENTITY(trans_struct, Subss_xmm15_dword_rsp);
            break;
        }

        case MUL:
        {
            INIT_ENTITY(trans_struct, Mulss_xmm15_dword_rsp);
            break;
        }

        case DIV:
        {
            INIT_ENTITY(trans_struct, Divss_xmm15_dword_rsp);
            break;
        }

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }
    
    INIT_ENTITY(trans_struct, Add_rsp_8);

    // Store result in stack
    INIT_ENTITY(trans_struct, Movss_dword_rsp_xmm15);

    // Restore xmm15 value
    INIT_ENTITY(trans_struct, Movd_xmm15_r15d);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _trans_hlt    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    INIT_ENTITY(trans_struct, Restore_regs);
    INIT_ENTITY(trans_struct, Return);

    // // xor rdi, rdi; mov rax, 3Ch; syscall
    // INIT_ENTITY(trans_struct, Exit);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _init_and_patch_cvtss2si(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned char reg_number = get_unsigned_char_from_input(trans_struct) - 1;

    INIT_ENTITY(trans_struct, Cvtss2si_r13d_xmmi);

    unsigned char patch_byte = (reg_number > 7)? 0x45: 0x44;
    unsigned int  patch_pos  = 1;
    unsigned int  patch_size = 1;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, &patch_byte);

    patch_byte = 0xE8 + ( (reg_number > 7)? reg_number - 8: reg_number);
    patch_pos  = 4;
    patch_size = 1;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, &patch_byte);

    return 0;
}

//-----------------------------------------------

int _init_and_patch_push_xmmi(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned char reg_number = get_unsigned_char_from_input(trans_struct) - 1;

    if (reg_number > 7)
        INIT_ENTITY(trans_struct, Push_xmmi_h)
    else
        INIT_ENTITY(trans_struct, Push_xmmi_l);

    unsigned char patch_byte = 0x04 + 0x08 * ( (reg_number > 7)? reg_number - 8 : reg_number);
    unsigned int  patch_pos  = (reg_number > 7)? 8: 7;
    unsigned int  patch_size = 1;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, &patch_byte);

    return 0;
}

//-----------------------------------------------

int _init_and_patch_pop_xmmi(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned char reg_number = get_unsigned_char_from_input(trans_struct) - 1;

    if (reg_number > 7)
        INIT_ENTITY(trans_struct, Pop_xmmi_h)
    else
        INIT_ENTITY(trans_struct, Pop_xmmi_l);

    unsigned char patch_byte = 0x04 + 0x08 * ( (reg_number > 7)? reg_number - 8 : reg_number);
    unsigned int  patch_pos  = (reg_number > 7)? 4: 3;
    unsigned int  patch_size = 1;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, &patch_byte);

    return 0;
}

//-----------------------------------------------

int _init_and_patch_mov_r13d_0(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int ram_index = (unsigned int) get_float_from_input(trans_struct);

    INIT_ENTITY(trans_struct, Mov_r13d_0);

    unsigned char* patch_data = (unsigned char*)&ram_index;
    unsigned int   patch_pos  = 2;
    unsigned int   patch_size = 4;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, patch_data);

    return 0;
}

//-----------------------------------------------

unsigned char _get_unsigned_char_from_input(Trans_struct* trans_struct
                                               FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned char value = 0;

    value = * (trans_struct->input.buffer
            +  trans_struct->input.pos);

    trans_struct->input.pos += 1;

    return value;
}

//-----------------------------------------------

float _get_float_from_input(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    float value = 0;

    value = * (float*) (trans_struct->input.buffer
                     +  trans_struct->input.pos); 

    trans_struct->input.pos += sizeof(float);

    return value;
}

//-----------------------------------------------

unsigned int _get_unsigned_int_from_input(Trans_struct* trans_struct 
                                             FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int value = 0;

    value = * (unsigned int*) (trans_struct->input.buffer
                            +  trans_struct->input.pos); 

    trans_struct->input.pos += sizeof(unsigned int);

    return value;
}

//-----------------------------------------------

int _trans_push   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    unsigned char oper_code = get_unsigned_char_from_input(trans_struct);

    switch (oper_code & ~ OPER_CODE_MASK)
    {
        case REGISTER_MASK | RAM_MASK:
        {
            int ret_val = init_and_patch_cvtss2si(trans_struct);
            if (ret_val == -1) return -1;

            INIT_ENTITY(trans_struct, Push_qword_4_r13d_plus_ADDR);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        case REGISTER_MASK:
        {
            int ret_val = init_and_patch_push_xmmi(trans_struct);
            if (ret_val == -1) return -1;

            break;
        }

        case IMM_MASK | RAM_MASK:
        {
            int ret_val = init_and_patch_mov_r13d_0(trans_struct);
            if (ret_val == -1) return -1;

            INIT_ENTITY(trans_struct, Push_qword_4_r13d_plus_ADDR);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        case IMM_MASK:
        {
            float imm_value = get_float_from_input(trans_struct);

            INIT_ENTITY(trans_struct, Push_qword_ADDR);
            ADD_PATCH_CONST(trans_struct, imm_value);
            //needs to be patched

            break;
        }

        case IMM_MASK | REGISTER_MASK | RAM_MASK:
        {
            int ret_val = init_and_patch_cvtss2si(trans_struct);
            if (ret_val == -1) return -1;

            unsigned int ram_index = 4 * (unsigned int) get_float_from_input(trans_struct);

            INIT_ENTITY(trans_struct, Mov_r14d_0);

            unsigned char* patch_data = (unsigned char*) &ram_index;
            unsigned int   patch_pos  = 2;
            unsigned int   patch_size = 4;

            PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, patch_data);

            INIT_ENTITY(trans_struct, Push_qword_4_r13d_plus_r14d_plus_ADDR);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    return 0;
}

//-----------------------------------------------

int _trans_pop    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    unsigned char oper_code = get_unsigned_char_from_input(trans_struct);

    switch (oper_code & ~ OPER_CODE_MASK)
    {
        case REGISTER_MASK | RAM_MASK:
        {
            int ret_val = init_and_patch_cvtss2si(trans_struct);
            if (ret_val == -1) return -1;

            INIT_ENTITY(trans_struct, Pop_r14);

            INIT_ENTITY(trans_struct, Mov_dword_ADDR_plus_4_r13d_r14d);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        case REGISTER_MASK:
        {
            int ret_val = init_and_patch_pop_xmmi(trans_struct);
            if (ret_val == -1) return -1;

            break;            
        }

        case IMM_MASK | RAM_MASK:
        {
            int ret_val = init_and_patch_mov_r13d_0(trans_struct);
            if (ret_val == -1) return -1;

            INIT_ENTITY(trans_struct, Pop_r14);

            INIT_ENTITY(trans_struct, Mov_dword_ADDR_plus_4_r13d_r14d);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        case IMM_MASK | REGISTER_MASK | RAM_MASK:
        {

            int ret_val = init_and_patch_cvtss2si(trans_struct);
            if (ret_val == -1) return -1;

            unsigned int ram_index = 4 * (unsigned int) get_float_from_input(trans_struct);

            INIT_ENTITY(trans_struct, Mov_r15d_0);

            unsigned char* patch_data = (unsigned char*)&ram_index;
            unsigned int   patch_pos  = 2;
            unsigned int   patch_size = 4;

            PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, patch_data);

            INIT_ENTITY(trans_struct, Pop_r14);

            INIT_ENTITY(trans_struct, Mov_dword_ADDR_plus_4_r13d_plus_r15d_r14d);
            ADD_PATCH_RAM_START_ADDR(trans_struct);
            // needs to be patched

            break;
        }

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    return 0;
}

//-----------------------------------------------

int _trans_out    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // Save xmm0
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);

    // movss xmm0, dword[rsp]; add rsp, 8;
    INIT_ENTITY(trans_struct, Pop_dword_xmm0);

    // push xmm1 - xmm15 to stack
    INIT_ENTITY(trans_struct, Push_xmms_1_15);

    // align stack to 16 boundary
    INSERT_NOPS(trans_struct);
    INIT_ENTITY(trans_struct, Align_stack_to_16);

    INIT_ENTITY(trans_struct, Rel_call);
    ADD_PATCH_STD_FUNC(trans_struct, OUT);
    // needs to be patched

    // sub rsp, 8 (if stack was not aligned to 16 boundary)
    INIT_ENTITY(trans_struct, Sub_stack_alignment);

    // pop xmm1 - xmm15
    INIT_ENTITY(trans_struct, Pop_xmms_1_15);
    //restore xmm0
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _trans_in     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // Save xmm0 value 
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);

    // push xmm1 - xmm7
    INIT_ENTITY(trans_struct, Push_xmms_1_15);

    // align stack to 16 boundary
    INSERT_NOPS(trans_struct);
    INIT_ENTITY(trans_struct, Align_stack_to_16);

    INIT_ENTITY(trans_struct, Rel_call);
    ADD_PATCH_STD_FUNC(trans_struct, IN);
    // needs to be patched

    // pop xmm0 - xmm7
    INIT_ENTITY(trans_struct, Pop_xmms_1_15);

    // sub rsp, 8 (if stack was not aligned to 16 boundary)
    INIT_ENTITY(trans_struct, Sub_stack_alignment);

    // sub rsp, 8; movss dword [rsp], xmm0
    INIT_ENTITY(trans_struct, Push_dword_xmm0);

    // Restore xmm0 value
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);
    

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _trans_pow    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // save xmm0, xmm1 values in regular registers
    INIT_SAVE_XMM_0_1(trans_struct);

    // xmm0 = base; xmm1 = exp;
    INIT_ENTITY(trans_struct, Movss_xmm0_dword_rsp_plus_8);
    INIT_ENTITY(trans_struct, Movss_xmm1_dword_rsp);

    INIT_ENTITY(trans_struct, Add_rsp_16);

    // push xmm1 - xmm7
    INIT_ENTITY(trans_struct, Push_xmms_1_15);

    INIT_ENTITY(trans_struct, Rel_call);
    ADD_PATCH_STD_FUNC(trans_struct, POW);

    // pop xmm1 - xmm7
    INIT_ENTITY(trans_struct, Pop_xmms_1_15);

    // needs to be patched
    INIT_ENTITY(trans_struct, Push_dword_xmm0);

    // restore 
    INIT_RESTORE_XMM_0_1(trans_struct);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _patch_short_cond_jump(Trans_entity* trans_entity, 
                           unsigned char patch_byte FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_entity);

    unsigned int patch_size = 1;
    unsigned int patch_pos  = 1;

    PATCH_ENTITY(trans_entity, patch_pos, patch_size, &patch_byte);

    return 0;
}

//-----------------------------------------------

int _patch_near_cond_jump(Trans_struct* trans_struct,
                          unsigned int patch_bytes FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int   patch_size = 4;
    unsigned int   patch_pos  = 1;
    unsigned char* patch_data = (unsigned char*) &patch_bytes;

    PATCH_ENTITY(LAST_ENTITY, patch_pos, patch_size, patch_data);

    return 0;
}

//-----------------------------------------------

int _trans_compare(Trans_struct* trans_struct, unsigned char op_code 
                                             FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // Save xmm0, xmm13 values
    INIT_SAVE_XMM_0_13(trans_struct);

    INIT_ENTITY(trans_struct, Null_xmm13);

    // Get first arg in xmm0
    INIT_ENTITY(trans_struct, Movss_xmm0_dword_rsp);
    INIT_ENTITY(trans_struct, Comiss_xmm0_dword_rsp_plus_8);

    switch(op_code)
    {
        case MR:    INIT_ENTITY(trans_struct, Jbe_short_ahead_N);   break;

        case MRE:   INIT_ENTITY(trans_struct, Jb_short_ahead_N);    break;

        case LS:    INIT_ENTITY(trans_struct, Jae_short_ahead_N);   break;

        case LSE:   INIT_ENTITY(trans_struct, Ja_short_ahead_N);    break;

        case EQ:    INIT_ENTITY(trans_struct, Jne_short_ahead_N);   break;

        case NEQ:   INIT_ENTITY(trans_struct, Je_short_ahead_N);    break;

        default:
        {
            error_report(INV_OPER_CODE);
            return -1;
        }
    }

    patch_short_cond_jump(LAST_ENTITY, 0x0A);

    INIT_ENTITY(trans_struct, Movss_xmm13_ADDR);
    ADD_PATCH_CONST(trans_struct, 1.0f);
    // needs to be patched

    // Clear stack from comparing values
    INIT_ENTITY(trans_struct, Add_rsp_16);

    INIT_ENTITY(trans_struct, Push_dword_xmm13);

    // Restore xmm0, xmm13 values
    INIT_RESTORE_XMM_0_13(trans_struct);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _trans_jmp(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    INIT_ENTITY(trans_struct, Near_rel_jmp_N); 

    trans_struct->input.pos += 1;
    unsigned int inp_dst = get_unsigned_int_from_input(trans_struct);

    int ret_val = add_jump_entity(&trans_struct->jumps, LAST_ENTITY, 
                                   trans_struct->result.cur_pos,
                                                        inp_dst);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _trans_cond_jmp(Trans_struct* trans_struct, unsigned char op_code
                                              FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    INIT_ENTITY(trans_struct, Add_rsp_16);

    // save xmm0 
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);

    // load first comparing value
    INIT_ENTITY(trans_struct, Movss_xmm0_dword_rsp_minus_16);

    // compare values
    INIT_ENTITY(trans_struct, Comiss_xmm0_dword_rsp_minus_8);

    // restore xmm0
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);

    switch(op_code)
    {
        case JA : INIT_ENTITY(trans_struct, Jbe_short_ahead_N); break; 

        case JAE: INIT_ENTITY(trans_struct,  Jb_short_ahead_N); break; 

        case JB : INIT_ENTITY(trans_struct, Jae_short_ahead_N); break; 

        case JBE: INIT_ENTITY(trans_struct,  Ja_short_ahead_N); break; 

        case JE : INIT_ENTITY(trans_struct, Jne_short_ahead_N); break; 

        case JNE: INIT_ENTITY(trans_struct,  Je_short_ahead_N); break; 
 
        default:
        {
            error_report(INV_OPER_CODE);
            return -1;
        }
    }

    int ret_val = patch_short_cond_jump(LAST_ENTITY, 0x05);
    if (ret_val == -1) return -1;

    INIT_ENTITY(trans_struct, Near_rel_jmp_N);

    trans_struct->input.pos += 1;
    unsigned int inp_dst = get_unsigned_int_from_input(trans_struct);

    ret_val = add_jump_entity(&trans_struct->jumps, LAST_ENTITY, 
                               trans_struct->result.cur_pos,
                                                    inp_dst);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _trans_std_func(Trans_struct* trans_struct, unsigned char op_code
                                              FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    // Save xmm0
    INIT_ENTITY(trans_struct, Movd_r15d_xmm0);

    // movss xmm0, dword[rsp]; add rsp, 8;
    INIT_ENTITY(trans_struct, Pop_dword_xmm0);

    // push xmm1 - xmm7 to stack
    INIT_ENTITY(trans_struct, Push_xmms_1_15);

    // align stack to 16 boundary
    INSERT_NOPS(trans_struct);
    INIT_ENTITY(trans_struct, Align_stack_to_16);

    INIT_ENTITY(trans_struct, Rel_call);
    ADD_PATCH_STD_FUNC(trans_struct, op_code);
    // needs to be patched

    // sub rsp, 8 (if stack was not aligned to 16 boundary)
    INIT_ENTITY(trans_struct, Sub_stack_alignment);

    // pop xmm1 - xmm7
    INIT_ENTITY(trans_struct, Pop_xmms_1_15);
    // push xmm0
    INIT_ENTITY(trans_struct, Push_dword_xmm0);

    //restore xmm0
    INIT_ENTITY(trans_struct, Movd_xmm0_r15d);

    trans_struct->input.pos += 1;

    return 0;
}

//-----------------------------------------------

int _trans_draw   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    return 0;
}

//-----------------------------------------------

int _trans_call   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    INIT_ENTITY(trans_struct, Rel_call);

    trans_struct->input.pos += 1;
    unsigned int inp_dst = get_unsigned_int_from_input(trans_struct);

    int ret_val = add_jump_entity(&trans_struct->jumps, LAST_ENTITY, 
                                   trans_struct->result.cur_pos,
                                                        inp_dst);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _trans_ret    (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    INSERT_NOPS(trans_struct);

    INIT_ENTITY(trans_struct, Return);

    trans_struct->input.pos += 1;

    return 0;
}