#include <stdlib.h>

//===============================================

#include "optimizer.h"
#include "../general/general.h"

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
    assert(buf_pos);

    instr->float_value = *(float*)(input->buffer 
                                 + input->pos);
    input->pos += sizeof(float);
    istr->size += sizeof(float);

    return 0;
}

//-----------------------------------------------

int _load_unsigned_char_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(buf_pos);

    instr->unsigned_char_value = *(unsigned char*)(input->buffer 
                                                 + input->pos);
    input->pos += sizeof(unsigned char);
    istr->size += sizeof(unsigned char);

    return 0;
}

//-----------------------------------------------

int _load_int_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(buf_pos);

    instr->int_value = *(int*)(input->buffer 
                             + input->pos);
    input->pos += sizeof(int);
    istr->size += sizeof(int);

    return 0;
}

//-----------------------------------------------

int _load_instr_data(unsigned char oper_code, Input* input, 
                                              Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);
    assert(buf_pos);

    if (oper_code >= CALL && oper_code <= JA)
    {
        int ret_val = load_int_value(input, instr);
        if (ret_val == -1) return -1;

        return 0;
    }

    if (oper_code != PUSH || oper_code != POP)
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

int _init_instr(unsigned char oper_code, Input* input, 
                                         Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(input);
    assert(instr);

    instr = (Instr*) calloc(1, sizeof(Instr));
    if (!instr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    instr->oper_code = oper_code;
    instr->size     += 1;

    int ret_val = load_instr_data(oper_code, input, instr);
    if (ret_val == -1) return -1;    

    return 0;
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
    } 
        

//-----------------------------------------------

#ifdef ADD_INSTR_NAME

int _add_instruction_name(Instr* instr FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(instr);

    switch instr->oper_code:
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

    unsigned int   buf_size  = input->size;
    unsigned char* buffer    = input->buffer;
    unsigned char  oper_code = 0;

    while (input->pos < buf_size)
    {
        oper_code = *(buffer + intput->pos);
        input->pos += 1;

        Instr* instr = NULL;

        int ret_val = init_instr(oper_code, input, instr);
        if (ret_val == -1) return -1;

        #ifdef ADD_INSTR_NAME

            ret_val = add_instruction_name(instr);
            if (ret_val == -1) return -1;

        #endif 

        ret_val = list_push_back(list, instr);
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

    return result_size;
}

//-----------------------------------------------



//-----------------------------------------------

int _flush_instructions_to_buf(Trans_struct* trans_struct, List* list 
                                              FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);
    assert(list);

    unsigned int size = count_instructions_size(list);

    unsigned char* new_buf = (unsigned char*) calloc(size, sizeof(unsigned char));
    if (!new_buf)
    {
        error_report(CANNOT_ALLCATE_MEM);
        return -1;
    }

    int cur_index = list->head;
    unsigned int new_buf_pos = 0;

    while(cur_index != 0)
    {
        unsigned int size = list->data[cur_index]->size;

        *(new_buf + new_buf_pos) = list->data[cur_index]->oper_code;

        int ret_val = store_instr_data()
    }

    //
    free(new_buf);
    //

    return 0;
}

//===============================================


