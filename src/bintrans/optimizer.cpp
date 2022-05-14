#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <math.h>

//===============================================

#include "optimizer.h"
#include "../general/general.h"
#include "../../lang/proc/assembler/processor_general.h"

#include "instr.h"

//===============================================

int _binary_optimize(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    List list = { 0 };

    int ret_val = list_ctor(&list);
    if (ret_val == -1) return -1;

    struct Dynamic_array jump_dst = { 0 };

    ret_val = dynamic_array_ctor(&jump_dst);
    if (ret_val == -1) return -1;

    ret_val = read_instructions(&trans_struct->input, 
                                &list, &jump_dst);
    if (ret_val == -1) return -1;

    ret_val = insert_labels_in_list(&list, &jump_dst);
    if (ret_val == -1) return -1;

    ret_val = optimize_instructions(&list);
    if (ret_val == -1) return -1;

    ret_val = recalculate_jump_dst(&list, &jump_dst);
    if (ret_val == -1) return -1;

    ret_val = dynamic_array_dtor(&jump_dst);
    if (ret_val == -1) return -1;

    ret_val = flush_instructions_to_buf(trans_struct, 
                                        &list);
    if (ret_val == -1) return -1;

    ret_val = list_free_instr(&list);
    if (ret_val == -1) return -1;

    ret_val = list_dtor(&list);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _list_free_instr(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;

    while (cur_index != 0)
    {
        #ifdef ADD_INSTR_NAME

            free(list->data[cur_index]->name_str);

        #endif 

        free(list->data[cur_index]);

        cur_index = list->next[cur_index];
    }

    return 0;
}

//-----------------------------------------------

int _load_float_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(input);

    instr->data.float_value = *(float*)(input->buffer 
                                      + input->pos);
    input->pos  += sizeof(float);
    instr->size += sizeof(float);

    return 0;
}

//-----------------------------------------------

int _load_unsigned_char_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(input);

    instr->data.unsigned_char_value = *(unsigned char*)(input->buffer 
                                                      + input->pos);
    input->pos  += sizeof(unsigned char);
    instr->size += sizeof(unsigned char);

    return 0;
}

//-----------------------------------------------

int _load_int_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(input);

    instr->data.int_value = *(int*)(input->buffer 
                                  + input->pos);
    input->pos  += sizeof(int);
    instr->size += sizeof(int);

    return 0;
}

//-----------------------------------------------

int _load_instr_data(unsigned char oper_code, Input* input, 
                     Instr* instr, Dynamic_array* jump_dst FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(input);

    unsigned char masks = oper_code & ~ OPER_CODE_MASK;
              oper_code = oper_code &   OPER_CODE_MASK;

    if (oper_code >= CALL && oper_code <= JA)
    {
        int ret_val = load_int_value(input, instr);
        if (ret_val == -1) return -1;

        ret_val = dynamic_array_add(jump_dst, instr->data.int_value, instr);
        if (ret_val == -1) return -1;

        return 0;
    }

    if (oper_code != PUSH && oper_code != POP)
        return 0;

    switch(masks)
    {
        case REGISTER_MASK | RAM_MASK: [[fallthrough]];

        case REGISTER_MASK:
        {
            int ret_val = load_unsigned_char_value(input, instr);
            if (ret_val == -1) return -1;

            break;
        }

        case IMM_MASK:                 [[fallthrough]];

        case IMM_MASK | RAM_MASK:
        {
            int ret_val = load_float_value(input, instr);
            if (ret_val == -1) return -1;

            break;
        }

        case IMM_MASK | RAM_MASK | REGISTER_MASK:
        {
            int ret_val = load_unsigned_char_value(input, instr);
            if (ret_val == -1) return -1;

            ret_val = load_float_value(input, instr);
            if (ret_val == -1) return -1;

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

Instr* _init_instr(unsigned char oper_code, Input* input, Dynamic_array* jump_dst
                                                          FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(input);

    Instr* instr = (Instr*) calloc(1, sizeof(Instr));
    if (!instr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    instr->oper_code = oper_code;
    instr->size     += 1;

    int ret_val = load_instr_data(oper_code, input, instr, jump_dst);
    if (ret_val == -1) return NULL;    

    return instr;
}

//-----------------------------------------------

#define DEF_OPER(name, code)                                \
                                                            \
    case code:                                              \
    {                                                       \
                                                            \
        instr->name_str = strdup(#name);                    \
        assert(instr->name_str);                            \
                                                            \
        break;                                              \
    } 
        

//-----------------------------------------------

#ifdef ADD_INSTR_NAME

int _add_instruction_name(Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);

    switch (instr->oper_code & OPER_CODE_MASK)
    {
        #include "../../text_files/commands_and_jumps.txt"

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    return 0;
}

#endif 

//-----------------------------------------------

#undef DEF_OPER

//-----------------------------------------------

int _read_instructions(Input* input, List* list, Dynamic_array* jump_dst 
                                                 FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(input);
    assert(list);

    unsigned int   buf_size   = input->size;
    unsigned char* buffer     = input->buffer;
    unsigned char  oper_code  = 0;

    input->pos += sizeof(struct Header);

    while (input->pos < buf_size)
    {
        oper_code = *(buffer + input->pos);
        input->pos += 1;

        Instr* instr = init_instr(oper_code, input, jump_dst);
        if (!instr) return -1;

        instr->inp_pos = input->pos;

        #ifdef ADD_INSTR_NAME

            int is_added = add_instruction_name(instr);
            if (is_added == -1) return -1;

        #endif 

        int ret_val = list_push_back(list, instr);
        if (ret_val == -1) return -1;
    }

    input->pos = 0;

    return 0;
}

//-----------------------------------------------

int _optimize_instructions(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int is_opt = 0;

    do 
    {
        is_opt = optimize_consts(list);
        if (is_opt == -1) return -1;

    } while(is_opt == 1);

    return 0;
}

//-----------------------------------------------

int _optimize_consts(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int is_opt_std_func     = optimize_std_func(list);
    if (is_opt_std_func     == -1) return -1;

    int is_opt_mul_zero     = optimize_mul_zero(list);
    if (is_opt_mul_zero     == -1) return -1;

    int is_opt_add_sub_zero = optimize_add_sub_zero(list);
    if (is_opt_add_sub_zero == -1) return -1;

    int is_opt_mul_one      = optimize_mul_one(list);
    if (is_opt_mul_one      == -1) return -1;

    int is_opt_arithm       = optimize_arithm(list);
    if (is_opt_arithm       == -1) return -1;


    return (is_opt_std_func || is_opt_arithm   || is_opt_mul_one
                            || is_opt_mul_zero || is_opt_add_sub_zero);
}

//-----------------------------------------------

int _optimize_std_func(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;
    int is_opt    = 0;

    while(cur_index != 0)
    {
        int nxt_index = list->next[cur_index];
        if (nxt_index == 0) break;

        unsigned char nxt_index_oper_code = list->data[nxt_index]->oper_code;

        if (list->data[cur_index]->oper_code == (PUSH | IMM_MASK)
        &&  SIN <= nxt_index_oper_code
        &&  ATG >= nxt_index_oper_code)
        {
            cur_index = fold_std_func(list, cur_index, nxt_index);
            if (cur_index == -1) return -1;

            is_opt = 1;
        }

        else 
            cur_index = nxt_index;
    }

    return is_opt;
}

//-----------------------------------------------

int _fold_std_func(List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    Instr* push = list->data[cur_index];
    Instr* func = list->data[nxt_index];

    float val = push->data.float_value;
    float res = 0;
    
    unsigned char func_code = func->oper_code;

    switch(func_code)
    {
        case SIN:   res = sinf (val);   break;

        case COS:   res = cosf (val);   break;

        case TG:    res = tanf (val);   break;

        case LN:    res = logf (val);   break;

        case ASIN:  res = asinf(val);   break;

        case ATG:   res = atanf(val);   break;

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    push->data.float_value = res;

    int next_iter_index = list->next[nxt_index];

    int err = 0;
    list_pop_by_index(list, (unsigned int)nxt_index, &err);
    if (err == -1) return -1;

    #ifdef ADD_INSTR_NAME
    
        free(func->name_str);
    
    #endif 

    free(func);

    return next_iter_index;
}

//-----------------------------------------------

int _optimize_arithm(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;
    int is_opt    = 0;

    while(cur_index != 0)
    {
        int nxt_index = list->next[cur_index];
        if (nxt_index == 0) break;

        int nxt_nxt_index = list->next[nxt_index];
        if (nxt_nxt_index == 0) break;

        unsigned char nxt_nxt_oper_code = list->data[nxt_nxt_index]->oper_code;

        if (list->data[cur_index]->oper_code == (PUSH | IMM_MASK)
        &&  list->data[nxt_index]->oper_code == (PUSH | IMM_MASK)
        && ((nxt_nxt_oper_code >= ADD && nxt_nxt_oper_code <= DIV) 
        ||   nxt_nxt_oper_code == POW))
        {
            cur_index = fold_arithm(list, cur_index, nxt_index);
            if (cur_index == -1) return -1;

            is_opt = 1;
        }

        else
            cur_index = nxt_index;
    }

    return is_opt;
}

//-----------------------------------------------

int _fold_arithm(List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    Instr* frst_push = list->data[cur_index];
    Instr* scnd_push = list->data[nxt_index];

    int arithm_index = list->next[nxt_index];
    Instr* arithm    = list->data[arithm_index];

    float frst_val = frst_push->data.float_value;
    float scnd_val = scnd_push->data.float_value;
    float res      = 0;

    unsigned char arithm_code = arithm->oper_code;

    switch(arithm_code)
    {
        case ADD: res = frst_val + scnd_val;      break;

        case SUB: res = frst_val - scnd_val;      break;

        case MUL: res = frst_val * scnd_val;      break;

        case DIV: res = frst_val / scnd_val;      break;

        case POW: res = powf(frst_val, scnd_val); break;

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    frst_push->data.float_value = res;

    int next_iter_index = list->next[arithm_index];

    int err = 0;
    list_pop_by_index(list, (unsigned int)nxt_index,    &err);
    if (err == -1) return -1;

    list_pop_by_index(list, (unsigned int)arithm_index, &err);
    if (err == -1) return -1;

    #ifdef ADD_INSTR_NAME

        free(arithm->name_str);
        free(scnd_push->name_str);

    #endif 

    free(arithm);
    free(scnd_push);

    return next_iter_index;
}

//-----------------------------------------------

int _optimize_mul_zero(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;
    int is_opt    = 0;

    while(cur_index != 0)
    {
        int nxt_index = list->next[cur_index];
        if (nxt_index == 0) break;

        int nxt_nxt_index = list->next[nxt_index];
        if (nxt_nxt_index == 0) break;

        if (list->data[nxt_nxt_index]->oper_code != MUL)
        {
            cur_index = nxt_index;
            continue;
        }

        char first_cond = (((list->data[cur_index]->oper_code &  OPER_CODE_MASK) == PUSH
                          || list->data[cur_index]->oper_code                    == IN)

                          && list->data[nxt_index]->oper_code == (PUSH | IMM_MASK)
                          && list->data[nxt_index]->data.float_value == 0);

        char secnd_cond = (((list->data[nxt_index]->oper_code &  OPER_CODE_MASK) == PUSH
                          || list->data[nxt_index]->oper_code                    == IN)

                          && list->data[cur_index]->oper_code == (PUSH | IMM_MASK)
                          && list->data[cur_index]->data.float_value == 0);

        if (first_cond || secnd_cond)
        {
            cur_index = fold_mul_zero(list, cur_index, nxt_index);
            if (cur_index == -1) return -1;

            is_opt = 1;
        }

        else
            cur_index = nxt_index;
    }

    return is_opt;
}

//-----------------------------------------------

int _fold_mul_zero(List* list, int cur_index, int nxt_index FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int mul_index = list->next[nxt_index];
    int next_iter_index = list->next[mul_index];

    Instr* push_0 = list->data[cur_index];

    push_0->oper_code = PUSH | IMM_MASK;
    push_0->size = 1 + sizeof(float);
    push_0->data.float_value = 0;

    #ifdef ADD_INSTR_NAME

        free(push_0->name_str);
        push_0->name_str = strdup("PUSH");

    #endif 

    Instr* nxt = list->data[nxt_index];
    Instr* mul = list->data[mul_index];

    int err = 0;

    list_pop_by_index(list, (unsigned int) mul_index, &err);
    if (err == -1) return -1;

    list_pop_by_index(list, (unsigned int) nxt_index, &err);
    if (err == -1) return -1;

    #ifdef ADD_INSTR_NAME

        free(mul->name_str);
        free(nxt->name_str);

    #endif 

    free(mul);
    free(nxt);

    return next_iter_index;
}

//-----------------------------------------------

int _optimize_mul_one(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;
    int is_opt    = 0;

    while(cur_index != 0)
    {
        int nxt_index = list->next[cur_index];
        if (nxt_index == 0) break;

        int mul_index = list->next[nxt_index];
        if (mul_index == 0) break;

        if (list->data[mul_index]->oper_code != MUL)
        {
            cur_index = nxt_index;
            continue;
        }

        int one_index = 0;
        int snd_index = 0;

        if (list->data[cur_index]->oper_code == (PUSH | IMM_MASK)
        &&  list->data[cur_index]->data.float_value == 1)
        {
            one_index = cur_index;
            snd_index = nxt_index;
        }

        else 
        if (list->data[nxt_index]->oper_code == (PUSH | IMM_MASK)
        &&  list->data[nxt_index]->data.float_value == 1)
        {
            one_index = nxt_index;
            snd_index = cur_index;
        }

        else
        {
            cur_index = nxt_index;
            continue;
        }

        unsigned char snd_oper_code = list->data[snd_index]->oper_code & OPER_CODE_MASK;

        if (snd_oper_code == IN || snd_oper_code == PUSH)
        {
            cur_index = fold_mul_one(list, one_index, mul_index);
            if (cur_index == -1) return -1;

            is_opt = 1;
        }

        else
            cur_index = nxt_index;
    }

    return is_opt;
}

//-----------------------------------------------

int _optimize_add_sub_zero(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int cur_index = list->head;
    int is_opt    = 0;

    while(cur_index != 0)
    {
        int nxt_index = list->next[cur_index];
        if (nxt_index == 0) break;

        int oper_index = list->next[nxt_index];
        if (oper_index == 0) break;

        if (list->data[oper_index]->oper_code != ADD 
        &&  list->data[oper_index]->oper_code != SUB)
        {
            cur_index = nxt_index;
            continue;
        }

        int zero_index = 0;
        int scnd_index = 0;

        if (list->data[cur_index]->oper_code == (PUSH | IMM_MASK)
        &&  list->data[cur_index]->data.float_value == 0)
        {
            zero_index = cur_index;
            scnd_index = nxt_index;
        }

        else 
        if (list->data[nxt_index]->oper_code == (PUSH | IMM_MASK)
        &&  list->data[nxt_index]->data.float_value == 0)
        {
            zero_index = nxt_index;
            scnd_index = cur_index;
        }

        else
        {
            cur_index = nxt_index;
            continue;
        }
            
        unsigned char scnd_oper_code = list->data[scnd_index]->oper_code & OPER_CODE_MASK;

        if (scnd_oper_code == IN || scnd_oper_code == PUSH)
        {
            cur_index = fold_add_sub_zero(list, zero_index, oper_index);
            if (cur_index == -1) return -1;

            is_opt = 1;
        }

        else
            cur_index = nxt_index;
    }

    return is_opt;
}

//-----------------------------------------------

int _fold_mul_one(List* list, int one_index, int mul_index 
                                             FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int next_iter_index = list->next[mul_index];

    Instr* one = list->data[one_index];
    Instr* mul = list->data[mul_index];

    int err = 0;

    list_pop_by_index(list, (unsigned int) one_index, &err);
    if (err == -1) return -1;

    list_pop_by_index(list, (unsigned int) mul_index, &err);
    if (err == -1) return -1;

    #ifdef ADD_INSTR_NAME

        free(one->name_str);
        free(mul->name_str);

    #endif 

    free(mul);
    free(one);

    return next_iter_index;
}

//-----------------------------------------------

int _fold_add_sub_zero(List* list, int zero_index, int oper_index FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int next_iter_index = list->next[oper_index];

    Instr* zero = list->data[zero_index];
    Instr* oper = list->data[oper_index];

    int err = 0;

    list_pop_by_index(list, (unsigned int) zero_index, &err);
    if (err == -1) return -1;

    list_pop_by_index(list, (unsigned int) oper_index, &err);
    if (err == -1) return -1;

    #ifdef ADD_INSTR_NAME

        free(zero->name_str);
        free(oper->name_str);

    #endif 

    free(zero);
    free(oper);

    return next_iter_index;
}

//-----------------------------------------------

int _count_instructions_size(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    int result_size = 0;
    int cur_index = list->head;

    while (cur_index != 0)
    {
        result_size += list->data[cur_index]->size;
        cur_index    = list->next[cur_index];
    }

    return result_size;
}

//-----------------------------------------------

int _store_int_value(Instr* instr, unsigned char* new_buf,
                                   unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(new_buf_pos);
    assert(new_buf);
    assert(instr);

    *(int*)(new_buf
         + *new_buf_pos) = instr->data.int_value;

    *new_buf_pos += sizeof(int);

    return 0;
}

//-----------------------------------------------

int _store_float_value(Instr* instr, unsigned char* new_buf,
                                     unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(new_buf_pos);
    assert(new_buf);
    assert(instr);

    *(float*)(new_buf
           + *new_buf_pos) = instr->data.float_value;

    *new_buf_pos += sizeof(float);

    return 0;
}

//-----------------------------------------------

int _store_unsigned_char_value(Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(new_buf_pos);
    assert(new_buf);
    assert(instr);

    *(unsigned char*)(new_buf
                   + *new_buf_pos) = instr->data.unsigned_char_value;

    *new_buf_pos += sizeof(unsigned char);

    return 0;
}

//-----------------------------------------------

int _store_instr_data(Instr* instr, unsigned char* new_buf,
                                    unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(new_buf_pos);
    assert(new_buf);
    assert(instr);

    unsigned char oper_code = instr->oper_code;

    unsigned char masks = oper_code & ~ OPER_CODE_MASK;
              oper_code = oper_code &   OPER_CODE_MASK;

    if (oper_code >= CALL && oper_code <= JA)
    {
        int ret_val = store_int_value(instr, new_buf,
                                             new_buf_pos);
        if (ret_val == -1) return -1;

        return 0;
    }

    if (oper_code != PUSH && oper_code != POP)
        return 0;

    switch(masks)
    {
        case REGISTER_MASK | RAM_MASK: [[fallthrough]];

        case REGISTER_MASK:
        {
            int ret_val = store_unsigned_char_value(instr, new_buf,
                                                           new_buf_pos);
            if (ret_val == -1) return -1;

            break;
        }

        case IMM_MASK:                 [[fallthrough]];

        case IMM_MASK | RAM_MASK:
        {
            int ret_val = store_float_value(instr, new_buf,
                                                   new_buf_pos);
            if (ret_val == -1) return -1;

            break;
        }

        case IMM_MASK | RAM_MASK | REGISTER_MASK:
        {
            int ret_val = store_unsigned_char_value(instr, new_buf,
                                                           new_buf_pos);
            if (ret_val == -1) return -1;

            ret_val = store_float_value(instr, new_buf, 
                                               new_buf_pos);
            if (ret_val == -1) return -1;

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

int _change_and_store_header(unsigned char* new_buf, unsigned char* old_buf, 
                             unsigned int   new_size FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(new_buf);
    assert(old_buf);

    Header header = { 0 };

    int ret_val = fast_cpy((void*)&header, (void*)old_buf, sizeof(Header));
    if (ret_val == -1) return -1;

    header.file_size = new_size;

    ret_val = fast_cpy((void*)new_buf, (void*)&header, sizeof(header));
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _flush_instructions_to_buf(Trans_struct* trans_struct, List* list 
                                              FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);
    assert(list);

    unsigned int size = sizeof(Header) + count_instructions_size(list);

    unsigned char* old_buf = trans_struct->input.buffer;
    unsigned char* new_buf = (unsigned char*) calloc(size, sizeof(unsigned char));
    if (!new_buf)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    int is_ok = change_and_store_header(new_buf, old_buf, size);
    if (is_ok == -1) return -1;

    int cur_index = list->head;
    unsigned int new_buf_pos = sizeof(Header);

    while(cur_index != 0)
    {
        if (list->data[cur_index]->oper_code == Label_oper_code)
        {
            cur_index = list->next[cur_index];
            continue;
        }

        unsigned int size = list->data[cur_index]->size;

        *(new_buf + new_buf_pos) = list->data[cur_index]->oper_code;
        new_buf_pos += 1;

        int ret_val = store_instr_data(list->data[cur_index],
                                       new_buf,
                                       &new_buf_pos);
        if (ret_val == -1) return -1;

        cur_index = list->next[cur_index];
    }

    trans_struct->input.buffer = new_buf;
    trans_struct->input.size   = size;

    return 0;
}

//===============================================

int _insert_labels_in_list(List* list, Dynamic_array* jump_dst 
                                       FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(list);
    assert(jump_dst);

    for (unsigned int counter = 0;
                      counter < jump_dst->num;
                      counter ++)
    {
        unsigned int inp_dst = jump_dst->inp_dst[counter];

        Instr* label = init_label(inp_dst);
        if (!label) return -1;

        int cur_list_index = list->head;

        while(cur_list_index != 0)
        {
            if (list->data[cur_list_index]->inp_pos == inp_dst)
            {
                int ret_val = list_push_after_index(list, cur_list_index, label);
                if (ret_val == -1) return -1;

                break;
            }

            else
                cur_list_index = list->next[cur_list_index];
        }
    }

    return 0;
}

//-----------------------------------------------

int _recalculate_jump_dst(List* list, Dynamic_array* jump_dst FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    
    assert(list);
    assert(jump_dst);

    int          cur_list_index   = list->head;
    unsigned int position_counter = sizeof(Header);

    while(cur_list_index != 0)
    {
        Instr* cur_instr = list->data[cur_list_index];

        if (cur_instr->oper_code == Label_oper_code)
        {
            int old_jump_dst = cur_instr->inp_pos;

            int ret_val = replace_jump_dst(jump_dst, old_jump_dst,
                                                     position_counter);
            if (ret_val == -1) return -1;
        }

        position_counter += cur_instr->size;
        cur_list_index    = list->next[cur_list_index];
    }

    return 0;
}

//-----------------------------------------------

int _replace_jump_dst(Dynamic_array* jump_dst, int old_jump_dst, 
                                               int new_jump_dst FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(jump_dst);

    for (unsigned int counter = 0;
                      counter < jump_dst->num;
                      counter ++)
    {
        if (old_jump_dst == jump_dst->inp_dst[counter])
        {
            jump_dst->instr[counter]->data.int_value = new_jump_dst;
        }
    }

    return 0;
}
//-----------------------------------------------

Instr* _init_label(int inp_dst FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    Instr* label = (Instr*) calloc(1, sizeof(Instr));
    if (!label)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    label->oper_code = Label_oper_code;
    label->size      = 0;

    label->inp_pos   = inp_dst;

    #ifdef ADD_INSTR_NAME

        label->name_str = strdup("LABEL");

    #endif 

    return label;
}

//-----------------------------------------------

int _dynamic_array_ctor(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(dynamic_array);

    unsigned int* inp_dst = (unsigned int*) calloc(Dynamic_array_init_cap, 
                                                   sizeof(unsigned int));
    if (!inp_dst)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    Instr** instr = (Instr**) calloc(Dynamic_array_init_cap, 
                                     sizeof(Instr*));
    if (!instr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    dynamic_array->cap     = Dynamic_array_init_cap;
    dynamic_array->inp_dst = inp_dst;
    dynamic_array->instr   = instr;
    dynamic_array->num     = 0;

    return 0;
}

//-----------------------------------------------

int _dynamic_array_dtor(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(dynamic_array);

    free(dynamic_array->inp_dst);
    free(dynamic_array->instr);

    return 0;
}

//-----------------------------------------------

int _dynamic_array_increase(Dynamic_array* dynamic_array FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(dynamic_array);

    unsigned int prev_cap = dynamic_array->cap;
    unsigned int incr_cap = prev_cap * 2;

    unsigned int* prev_inp_dst = dynamic_array->inp_dst;
    unsigned int* incr_inp_dst = (unsigned int*) my_recalloc( (void* ) prev_inp_dst,
                                                              (size_t) incr_cap ,
                                                              (size_t) prev_cap ,
                                                               sizeof (unsigned int));
    if (!incr_inp_dst) return -1;

    Instr** prev_instr = dynamic_array->instr;
    Instr** incr_instr = (Instr**) my_recalloc( (void* ) prev_instr,
                                                (size_t) incr_cap ,
                                                (size_t) prev_cap ,
                                                 sizeof (Instr*));

    dynamic_array->inp_dst = incr_inp_dst;
    dynamic_array->instr   = incr_instr;
    dynamic_array->cap     = incr_cap;

    return 0;
}

//-----------------------------------------------

int _dynamic_array_add(Dynamic_array* dynamic_array, unsigned int inp_dst, Instr* instr 
                                                                FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(dynamic_array);

    if (dynamic_array->num == dynamic_array->cap)
    {
        int ret_val = dynamic_array_increase(dynamic_array);
        if (ret_val == -1) return -1;
    }

    dynamic_array->inp_dst[dynamic_array->num] = inp_dst;
    dynamic_array->instr  [dynamic_array->num] = instr;
    dynamic_array->num += 1;

    return 0;
}

//===============================================
