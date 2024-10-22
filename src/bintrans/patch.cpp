#include <stdlib.h>
#include <tgmath.h>
#include <math.h>

//===============================================

#include "patch.h"
#include "trans.h"
#include "../general/general.h"
#include "../../lang/proc/assembler/processor_general.h"
#include "instr.h"
#include "standard.h"

//===============================================

int _binary_patch_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int ret_val = call_buf_prepare(trans_struct);
    if (ret_val == -1) return -1;

    ret_val = ram_buffer_allocate(trans_struct);
    if (ret_val == -1) return -1;

    unsigned int cur_index = 0;

    for (;cur_index < trans_struct->patch.num ;
          cur_index ++ )
    {
        ret_val = patch_instruction(trans_struct, &trans_struct->patch.instructions[cur_index]);
        if (ret_val == -1) return -1;
    }

    ret_val = fix_up_jumps(&trans_struct->jumps,
                           &trans_struct->pos);
    if (ret_val == -1) return -1;

    ret_val = jumps_struct_dtor(&trans_struct->jumps);
    if (ret_val == -1) return -1;

    ret_val = pos_struct_dtor(&trans_struct->pos);
    if (ret_val == -1) return -1;

    int is_freed = free_patch_struct(&trans_struct->patch);
    if (is_freed == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _patch_instruction(Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_instr);

    int patch_type = patch_instr->patch_type;
    int ret_val = 0;

    switch (patch_type)
    {
        case RAM_START_ADDR:
        {
            ret_val = patch_ram_start_addr(trans_struct, patch_instr);
            if (ret_val == -1) return -1;

            break;
        }

        case STD_FUNC:
        {
            ret_val = patch_std_func(trans_struct, patch_instr);
            if (ret_val == -1) return -1;

            break;
        }

        default:
        {
            error_report(JIT_INV_PATCH_TYPE);
            return -1;
        }
    }

    return 0;
}

//-----------------------------------------------

#define DEF_STD_FUNC(name, code)                            \
                                                            \
    case code: destin_pos = (uint64_t) &standard_##name;    \
               break;                                       

//-----------------------------------------------

int _patch_std_func(Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_instr);

    uint64_t origin_pos = (uint64_t) patch_instr->res_buf_pos 
                                   + trans_struct->result.buffer_addr;
    uint64_t destin_pos = 0;

    switch (patch_instr->std_func_code)
    {

        #include "../../text_files/std_func.txt"

        default:
        {
            error_report(INV_STD_FUNC_CODE);
            return -1;
        }
    }

    int offset = (int) ((int64_t)destin_pos - (int64_t)origin_pos);

    unsigned int   patch_pos  = 1;
    unsigned int   patch_size = 4;
    unsigned char* patch_data = (unsigned char*) &offset;

    PATCH_ENTITY(patch_instr->entity, patch_pos, patch_size, patch_data);
    
    return 0;
}

//-----------------------------------------------

#undef DEF_STD_FUNC

//-----------------------------------------------

int _patch_ram_start_addr(Trans_struct* trans_struct, Patch_instr* patch_instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_instr);

    uint64_t ram_start_addr = (uint64_t) trans_struct->ram_buffer;

    unsigned int   patch_size = 4;
    unsigned char* patch_data = (unsigned char*) &ram_start_addr;
    unsigned int   patch_pos  = 5;

    PATCH_ENTITY(patch_instr->entity, patch_pos, patch_size, patch_data);

    return 0;
}

//-----------------------------------------------

int _init_patch_struct(Patch* patch_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_struct);

    patch_struct->num = 0;
    patch_struct->cap = Initial_patch_cap;

    Patch_instr* instructions = (Patch_instr*) calloc(Initial_patch_cap,
                                                      sizeof(Patch_instr));
    if (!instructions)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    patch_struct->instructions = instructions;
    patch_struct->ram_is_used   = 0;

    return 0;
}

//-----------------------------------------------

int _free_patch_struct(Patch* patch_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_struct);

    free(patch_struct->instructions);

    return 0;
}

//-----------------------------------------------

int _get_jump_res_dst(Trans_entity* trans_entity, Pos* pos, unsigned int patch_pos
                                                            FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(trans_entity);
    assert(pos);

    unsigned int inp_dst = *(unsigned int*) (trans_entity->data + patch_pos);

    size_t count = (size_t) pos->num;
    size_t size  = sizeof  (int);
    void*  data  = (void*) (pos->inp_pos);
    void*  key   = (void*) &inp_dst;

    void* result = bsearch(key, data, count, size, &int_compare);
    if (!result)
    {
        error_report(INP_DST_NOT_FOUND);
        return -1;
    }

    unsigned int index = (unsigned int) ( ( (uint64_t) result - (uint64_t) data ) >> 2 );
    return pos->res_pos[index];
}

//-----------------------------------------------

int _fix_up_jumps(Jumps* jumps, Pos* pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(jumps);

    unsigned int counter = 0;

    for (; counter < jumps->num; counter ++)
    {
        unsigned int patch_size = 4;
        unsigned int patch_pos  = 0;
        
        Trans_entity* cur_entity = jumps->entities[counter];

        if (cur_entity->type == Near_rel_jmp_N_type
        ||  cur_entity->type == Rel_call_type)

            patch_pos = 1;
        else 
            patch_pos = 2;

        unsigned int res_pos = jumps->jmp_pos[counter];

        int res_dst = get_jump_res_dst(cur_entity, pos, patch_pos);
        if (res_dst == -1) return -1;

        int offset = res_dst - (int) res_pos;
        unsigned char* patch_data = (unsigned char*) &offset;

        PATCH_ENTITY(cur_entity, patch_pos, patch_size, patch_data);
    }

    return 0;
}

//-----------------------------------------------

int _patch_instructions_resize(Patch* patch_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(patch_struct);

    unsigned int new_cap = patch_struct->cap * 2;

    Patch_instr* new_instructions_arr = (Patch_instr*) my_recalloc( (void*)  patch_struct->instructions, 
                                                                    (size_t) new_cap,
                                                                    (size_t) patch_struct->cap,
                                                                     sizeof (Patch_instr));
    if (!new_instructions_arr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    patch_struct->instructions = new_instructions_arr;
    patch_struct->cap          = new_cap;

    return 0;
}

//-----------------------------------------------

Patch_instr* _patch_instr_init(Trans_struct* trans_struct, int type FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    Patch* patch = &trans_struct->patch;

    if (patch->num == patch->cap)
    {
        int ret_val = patch_instructions_resize(patch);
        if (ret_val == -1) return NULL;
    }

    Patch_instr* cur_patch_instr = &patch->instructions[patch->num];

    cur_patch_instr->patch_type  = type; 
    cur_patch_instr->res_buf_pos = trans_struct->result.cur_pos;

    unsigned int cur_index  = trans_struct->entities.num - 1; 
    cur_patch_instr->entity = trans_struct->entities.data[cur_index];

    return cur_patch_instr;
}

//-----------------------------------------------

int _add_patch_instr_ram_start_addr(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    Patch_instr* patch_instr = patch_instr_init(trans_struct, RAM_START_ADDR);
    if (!patch_instr) return -1;

    trans_struct->patch.ram_is_used = 1;

    trans_struct->patch.num += 1;
    return 0;
}

//-----------------------------------------------

int _add_patch_instr_std_func(Trans_struct* trans_struct, int std_func_code 
                                                    FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    Patch_instr* cur_patch_instr = patch_instr_init(trans_struct, STD_FUNC);
    if (!cur_patch_instr) return -1;

    cur_patch_instr->std_func_code = std_func_code;

    trans_struct->patch.num += 1;
    return 0;
}

