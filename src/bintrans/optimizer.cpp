#include <stdlib.h>
#include <string.h>

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

    ret_val = read_instructions(&trans_struct->input, 
                                &list);
    if (ret_val == -1) return -1;

    //-------------------------------------------

    {
        unsigned int cur_index = list.head;

        while (cur_index != 0)
        {
            printf("\n index %d hex oper_code %x size: %d float: %x int: %x char: %x \n", cur_index, 
                                                                                          list.data[cur_index]->oper_code,
                                                                                          list.data[cur_index]->size,
                                                                                          (unsigned)list.data[cur_index]->data.float_value,
                                                                                          list.data[cur_index]->data.int_value,
                                                                                          list.data[cur_index]->data.unsigned_char_value);
        
            cur_index = list.next[cur_index];
        }

    }

    //-------------------------------------------

    ret_val = optimize_instructions(&list);
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
                                              Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(input);

    if (oper_code >= CALL && oper_code <= JA)
    {
        int ret_val = load_int_value(input, instr);
        if (ret_val == -1) return -1;

        return 0;
    }

    if (oper_code != PUSH && oper_code != POP)
        return 0;

    switch(oper_code & ~ OPER_CODE_MASK)
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

Instr* _init_instr(unsigned char oper_code, Input* input
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

    int ret_val = load_instr_data(oper_code, input, instr);
    if (ret_val == -1) return NULL;    

    return instr;
}

//-----------------------------------------------

#define DEF_OPER(name, code)                                \
                                                            \
    case code:                                              \
    {                                                       \
                                                            \
        unsigned int len = strlen(#name) + 1;               \
                                                            \
        instr->name_str = (char*) calloc(1, len);           \
        assert(instr->name_str);                            \
                                                            \
        int ret_val = fast_cpy((void*)instr->name_str,      \
                               (void*)#name,                \
                               len);                        \
        if (ret_val == -1) return -1;                       \
                                                            \
        break;                                              \
    } 
        

//-----------------------------------------------

#ifdef ADD_INSTR_NAME

int _add_instruction_name(Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);

    switch (instr->oper_code)
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

int _read_instructions(Input* input, List* list FOR_LOGS(, LOG_PARAMS))
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

        Instr* instr = init_instr(oper_code, input);
        if (!instr) return -1;

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

    int ret_val = optimize_reg_pops(list);
    if (ret_val == -1) return -1;

    int is_opt = 0;

    do 
    {
        is_opt = optimize_arithm(list);
        if (is_opt == -1) return -1;

    } while(is_opt == 1);

    return 0;
}

//-----------------------------------------------

int _optimize_arithm(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);



    return 0;
}

//-----------------------------------------------

int _optimize_reg_pops(List* list FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(list);

    float Registers[16] = { 0 };

    unsigned int cur_index = list->head;

    return 0;
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

    printf("\n counted size: %d \n", result_size);

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

    if (oper_code >= CALL && oper_code <= JA)
    {
        int ret_val = store_int_value(instr, new_buf,
                                             new_buf_pos);
        if (ret_val == -1) return -1;

        return 0;
    }

    if (oper_code != PUSH && oper_code != POP)
        return 0;

    switch(oper_code & ~ OPER_CODE_MASK)
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

    int is_ok = fast_cpy((void*) new_buf, (void*) old_buf, sizeof(Header));
    if (is_ok == -1) return -1;

    int cur_index = list->head;
    unsigned int new_buf_pos = sizeof(Header);

    while(cur_index != 0)
    {
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


