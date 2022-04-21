#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
//===============================================

#include "bintrans.h"
#include "instr.h"
#include "../general/general.h"
#include "../../lang/proc/assembler/processor_general.h"

//===============================================

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(binary_input);
    
    if (input_filename == NULL)
    {
        error_report(INPUT_INV_NAME);
        return -1;
    }

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

    long size = get_file_size(binary_input->file_ptr);
    if (size == -1) return -1;

    binary_input->size = (unsigned int)size;

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

    unsigned int ret_val = (unsigned int) fread(buffer, sizeof(char), binary_input->size, 
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

int _increase_entities_array(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    size_t old_cap = (size_t)trans_struct->cap;
    size_t new_cap = old_cap * 2;

    Trans_entity* old_array = trans_struct->entities;
    Trans_entity* new_array = (Trans_entity*) 
                              my_recalloc((void*)old_array, new_cap, 
                                     old_cap, sizeof(Trans_entity));

    if (!new_array) return -1;

    trans_struct->entities = new_array;
    trans_struct->cap      = (unsigned int) new_cap;

    return 0;
}

//-----------------------------------------------

int _binary_translate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    TRANS_STRUCT_VALID(trans_struct);

    int is_ok = binary_header_check(trans_struct);
    if (is_ok == -1) return -1;

    trans_struct->buffer_pos += (unsigned int)sizeof(Header);

    int ret_val = init_entity(&trans_struct->entities[trans_struct->num],
                               Save_regs_size, 
                               Save_regs);

    if (ret_val == -1) return -1;

    ret_val = translate_instructions(trans_struct);
    if (ret_val == -1) return -1;

    int ret_val = init_entity(&trans_struct->entities[trans_struct->num],
                               Restore_regs_size, 
                               Restore_regs);
    
    if (ret_val == -1) return -1;              

    return 0;
}

//-----------------------------------------------

int _translate_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int ret_val = 0;

    while (trans_struct->buffer_pos < trans_struct->input_size)
    {
        ret_val = translate_single_instruction(trans_struct);
        if (ret_val == -1) return -1;

        TRANS_STRUCT_VALID(trans_struct);

        if (trans_struct->num == trans_struct->size)
        {
            ret_val = increase_entities_array(trans_struct);
            if (ret_val == -1) return -1;
        }
    }

    return 0;
}

//-----------------------------------------------



//-----------------------------------------------

int _translate_single_instruction(Trans_struct* trans_struct, FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    Trans_entity* trans_entity = &trans_struct->entities[trans_struct->num];

    

    trans_struct->num++;

    return 0;   
}

//-----------------------------------------------

int _init_entity(Trans_entity* trans_entity, unsigned int size, unsigned char* data FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_entity);

    trans_entity->size = size;

    int ret_val = fast_cpy((void*) trans_entity->data,
                           (void*) data,
                                   size);

    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _binary_header_check(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    Header header = { 0 };
    
    memcpy(&header, trans_struct->input_buffer, sizeof(Header));

    int err_num = 0;

    if (header.signature != SIGN)
    {    
        error_report(HDR_INV_SIGN);
        err_num++;
    }

    if (header.version   != VERSION)
    {
        error_report(HDR_INV_VERSION);
        err_num++;
    }

    if (header.file_size != trans_struct->input_size)
    { 
        error_report(HDR_INV_FILE_SIZE);
        err_num++;
    }

    if (err_num) return -1;

    return 0;
}


//-----------------------------------------------

int _binary_execute(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int ret_val = flush_entities_to_buf(trans_struct);
    if (ret_val == -1) return -1;

    ret_val = call_buf_change_acc_prot(trans_struct, PROT_EXEC);
    if (ret_val == -1) return -1;

    ret_val = call_translated_code(trans_struct);
    if (ret_val == -1) return -1;

    ret_val = call_buf_change_acc_prot(trans_struct, PROT_READ | PROT_WRITE);
    if (ret_val == -1) return -1;
    
    free(trans_struct->call_buf);

    return 0;
}

//-----------------------------------------------

int _call_translated_code(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int (*func) (void) = NULL;
    func = (int (*)(void)) trans_struct->call_buf;

    int exit_code = func();
    printf(" Exit code of translated code: %d \n", exit_code);

    return 0;
}

//-----------------------------------------------

int _call_buf_change_acc_prot(Trans_struct* trans_struct, int prot FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int size = trans_struct->call_buf_size;

    int ret_val = mprotect((void*)trans_struct->call_buf, size, prot);
    if (ret_val != 0)
    {
        error_report(MPROTECT_ERR);
        return -1;
    }

    return 0;
}

//-----------------------------------------------

int _call_buf_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
    {
        error_report(SYSCONF_ERR);
        return -1;
    }

    unsigned int size = trans_struct->call_buf_size;

    trans_struct->call_buf = (unsigned char*) aligned_alloc (pagesize,  
                                        size * sizeof(unsigned char));
    if (!trans_struct->call_buf) return -1;

    return 0;
}

//-----------------------------------------------

int _flush_entities_to_buf(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    // count size of translated code 
    trans_struct->call_buf_size = 4;

    int ret_val = call_buf_allocate(trans_struct);
    if (ret_val == -1) return -1;

    // flushing

    trans_struct->call_buf[0] = 0x48;
    trans_struct->call_buf[1] = 0x31;
    trans_struct->call_buf[2] = 0xC0;
    trans_struct->call_buf[3] = 0xC3;

    // flushing end 

    return 0;
}

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

    trans_struct->entities = (Trans_entity*) calloc(Entities_init_cap, sizeof(Trans_entity));
    if (!trans_struct->entities) 
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    trans_struct->cap = Entities_init_cap;
    trans_struct->num = 0;

    #ifdef BINTRANS_LISTING

        int ret_val = init_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

int _init_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    FILE* listing = open_file(Listing_filename, "ab");
    if (!listing)
    {
        error_report(LISTING_CLOSE_ERR);
        return -1;
    }

    trans_struct->listing = listing;

    fprintf(listing, "\t Listing Start: ");
    fprintf(listing, "%s \n", get_time_string());

    return 0;
}

//-----------------------------------------------

int _end_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    fprintf(trans_struct->listing, "\t Listing End: ");
    fprintf(trans_struct->listing, "%s \n", get_time_string());

    int ret_val = close_file(trans_struct->listing);
    if (ret_val == -1)
    {
        error_report(LISTING_CLOSE_ERR);
        return -1;
    }

    return 0;
}

//-----------------------------------------------

int _trans_struct_dtor(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    // if (trans_struct->buffer_pos != trans_struct->input_size)
    // {
    //     error_report(TRANS_STRUCT_DTOR_EARLY);
    //     return -1;
    // }

    free(trans_struct->entities);

    #ifdef BINTRANS_LISTING

        int ret_val = end_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int err_num = 0;

    if (!trans_struct->entities)
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

    if (trans_struct->buffer_pos > trans_struct->input_size)
    {
        error_report(TRANS_STRUCT_INV_BUFFER_POS);
        err_num++;
    }

    #ifdef BINTRANS_LISTING

        if (trans_struct->listing == NULL)
        {
            error_report(LISTING_NULL_PTR);
            err_num++;
        }

    #endif 

    if (err_num != 0)
        return -1;

    return 0;
}

//===============================================
