#include <stdlib.h>

//===============================================

#include "bintrans.h"
#include "../general/general.h"

//===============================================

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(binary_input);
    assert(input_filename);

    FILE* input_file_ptr = open_file(input_filename, "rb");
    if (input_file_ptr == NULL)
        return -1;

    binary_input->file_ptr = input_file_ptr;

    int ret_val = input_load_to_buffer(binary_input);
    if (ret_val == -1) return -1;

    ret_val = close_file(input_file_ptr);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _free_binary_input(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    free(binary_input->buffer);

    return 0;
}

//-----------------------------------------------

int _input_load_to_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    unsigned int size = (unsigned int)get_file_size(binary_input->file_ptr);
    if (size == -1) return -1;

    binary_input->size = size;

    int ret_val = fill_input_buffer(binary_input);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _fill_input_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    char* buffer = (char*) calloc(binary_input->size, sizeof(char));
    if (!buffer)
    {
		error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    int ret_val = fread(buffer, sizeof(char), binary_input->size, 
                                              binary_input->file_ptr);
    if (ret_val != binary_input->size)
    {
        error_report(FREAD_ERR);
        return -1;
    }

    binary_input->buffer = buffer;
    return 0;
}

//-----------------------------------------------

int _binary_translate(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

}

//-----------------------------------------------

int _binary_execute(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();


}

//-----------------------------------------------

//-----------------------------------------------

int _trans_struct_ctor(Trans_struct* trans_struct, Binary_input* binary_input 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(trans_struct);
    assert(binary_input);

    trans_struct->input_buffer = binary_input->buffer;
    trans_struct->input_size   = binary_input->size; 
    trans_struct->buffer_pos   = 0;

    trans_struct->entities = (Entity*) calloc(Entities_init_cap, sizeof(Entity));
    if (!trans_struct->entities) 
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    trans_struct->entities_cap = Entities_init_cap
    trans_struct->entities_num = 0;

    return 0;
}

//-----------------------------------------------

int _trans_struct_dtor(Trans_struct* trans_strcut FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    if (trans_struct->buffer_pos != trans_struct->input_size)
    {
        error_report(TRANS_STRUCT_DTOR_EARLY);
        return -1;
    }

    free(trans_struct->entities);

    return 0;
}

//-----------------------------------------------

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int err_num = 0;

    if (!trans_struct->entites)
    {
        error_report(TRANS_STRUCT_NULL_ENTITIES);
        err_num++;
    }

    if (trans_struct->cap == 0)
    {
        error_report(TRANS_STRUCT_ZERO_CAP);
        err_num++;
    }    

    if (trans_struct->cap < trans_struct->num)
    {
        error_report(TRANS_STRUCT_CAP_LESS_NUM);
        err_num++;
    }

    if (!trans_struct->input_buffer)
    {
        error_report(TRANS_STRUCT_NULL_INPUT_BUFFER);
        err_num++;
    }

    if (trans_struct->buffer_pos < 0 
    ||  trans_struct->buffer_pos > trans_struct->input_size)
    {
        error_report(TRANS_STRUCT_INV_BUFFER_POS);
        err_num++;
    }

    if (err_num != 0)
        return -1;

    return 0;
}

//===============================================
