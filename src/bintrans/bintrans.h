#pragma once 

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

struct Binary_input 
{

    FILE* file_ptr;
    unsigned int size;

    const char* buffer;
};

//===============================================

struct Trans_entity
{
    char* data;
    unsigned int size;

    //type maybe?
};

//===============================================

struct Trans_struct
{
    Trans_entity* entities;
    unsigned int entities_number;

    unsigned int buffer_pos;
};

//===============================================

int _read_binary_input(const char* input_filename FOR_LOGS(, LOG_PARAMS));

int _binary_translate(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

int _trans_struct_ctor(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dtor(Trans_struct* trans_strcut FOR_LOGS(, LOG_PARAMS));

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

//===============================================

#define read_binary_input(filename) \
       _read_binary_input(filename FOR_LOGS(, LOG_ARGS)) 

#define trans_struct_ctor(trans_struct) \
       _trans_struct_ctor(trans_struct FOR_LOGS(, LOG_ARGS))

#define trans_struct_dtor(trans_struct) \
       _trans_struct_dtor(trans_struct FOR_LOGS(, LOG_ARGS))

#define trans_struct_validator(trans_struct) \
       _trans_struct_validator(trans_struct FOR_LOGS(, LOG_ARGS))

#define binary_translate(input) \
       _binary_translate(input FOR_LOGS(, LOG_ARGS))
