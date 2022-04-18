#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "processor.h"
#include "../../general/general.h"
#include "../assembler/config.h"

//===================================================================

static FILE* proc_output = stdout;

static FILE* proc_input = stdin;

//===================================================================

int _procstruct_ip_check(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    if (procstruct->ip - procstruct->code_array
        >procstruct->code_file_size
     || procstruct->code_array - procstruct->ip > 0)

        error_report(PROC_INV_IP);

    return 1;
}

//===================================================================

int proc_check_command_line_args(int argc) {

    if (argc != PROC_NUM_OF_CMND_LINE_ARGS) {

        fprintf(stderr, "Invalid number of command line arguments\n");
        return -1;
    }
    return 0;
}

//===================================================================

int _open_proc_input(const char* filename, LOG_PARAMS) {

    proc_log_report();

    if (filename == NULL) 
        error_report(INV_FILE_NAME);

    extern FILE* proc_input;

    FILE* fp = fopen(filename, "rb");

    if (fp == NULL) 
        error_report(FOPEN_ERROR);

    proc_input = fp;

    return 0;
}

//==================================================================

int _close_proc_input(LOG_PARAMS) {

    proc_log_report();

    extern FILE* proc_input;

    if (proc_input != stdin) {

        int fclose_ret = fclose(proc_input);

        if (fclose_ret == EOF)
            error_report(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================

int _open_proc_output(const char* filename, LOG_PARAMS) {

    proc_log_report();

    if (filename == NULL)
        error_report(INV_FILE_NAME);

    extern FILE* proc_output;

    FILE* fp = fopen(filename, "wb");

    if (fp == NULL)
        error_report(INV_FILE_PTR);

    proc_output = fp;

    return 0;
}

//===================================================================

int _close_proc_output(LOG_PARAMS) {

    proc_log_report();

    extern FILE* proc_output;

    if (proc_output != stdout) {

        int fclose_ret = fclose(proc_output);

        if (fclose_ret == EOF)
            error_report(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================

int _header_check(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    memcpy(&procstruct->header, 
            procstruct->code_array, 
            sizeof(struct Header));

    if (procstruct->header.signature != SIGN)
        error_report(HDR_INV_SIGN);

    if (procstruct->header.version != VERSION)
        error_report(HDR_INV_VERSION);

    if (procstruct->header.file_size != procstruct->code_file_size)
        error_report(HDR_INV_FILE_SIZE);

    return 0;
}

//===================================================================

int _procstruct_allocate_memory(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();
    
    elem_t* temp_ptr = (elem_t*)calloc(RAM_SIZE, sizeof(elem_t));

    if (temp_ptr == NULL)
        error_report(CANNOT_ALLOCATE_MEM);

    procstruct->ram = temp_ptr;

    return 0;
}

//===================================================================

int _procstruct_free_memory(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    if (procstruct->ram == NULL)
        error_report(PROC_RAM_IS_NULL);

    free(procstruct->ram);

    return 0;
}

//===================================================================

int _init_procstruct(struct Procstruct* procstruct, FILE* fp, LOG_PARAMS) {

    proc_log_report();

    FILE_PTR_CHECK(fp);
    PROCSTRUCT_PTR_CHECK(procstruct);

    long code_file_size = 0;

    const unsigned char* code_array = read_from_file(fp, &code_file_size);

    CHAR_PTR_CHECK(code_array);

    procstruct->code_array = code_array;
    procstruct->code_file_size = code_file_size;

    int header_check_ret = header_check(procstruct);
    if (header_check_ret == -1)
        return -1;

    procstruct->ip = procstruct->code_array + sizeof(struct Header);

    int ret = stack_ctor(&procstruct->stack);
    if (ret == -1)
        return -1;

    ret = procstruct_allocate_memory(procstruct);

    if (ret == -1)
        return -1;

    procstruct->video = procstruct->ram + VIDEO_MEMORY_ADDRESS;

    return 0;
}

//===================================================================


int _dtor_procstruct(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);
    PROCSTRUCT_IP_CHECK(procstruct);

    if (procstruct->code_array == NULL)
        error_report(INV_CODE_ARRAY_PTR);

    free((void*)(procstruct->code_array));

    int ret = procstruct_free_memory(procstruct);

    if (ret == -1)
        return -1;

    return 0;
}
//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, LOG_PARAMS) {

    proc_log_report();

    if (fp == NULL) {

        error_report(INV_FILE_PTR);
        return NULL;
    }

    long code_file_size = file_size_(fp);
    if ( code_file_size < 0)
        return NULL;

    if (size) *size = code_file_size;

    const unsigned char* code_array = 
    (const unsigned char*)copy_data_to_buf(code_file_size, fp, LOG_ARGS);

    if (code_array == NULL)
        return NULL;

    return code_array;
}

//===================================================================

int _proc_perform(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    while (1) {

        int ret_val = proc_execute_command(procstruct);

        if (ret_val == -1) {
            
            processor_dump(procstruct);
            return -1;
        }

        if (ret_val == 1)
            break;
    }

    int final_check_value = proc_final_check(procstruct);
    if (final_check_value == -1)
        return -1;

    return 0;
}

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions)               \
                                                                    \
    case code: {                                                    \
                                                                    \
        instructions                                                \
                                                                    \
        if (code == 0)                                              \
            return 1;                                               \
                                                                    \
        break;                                                      \
    }

//===================================================================

#define DEF_JMP_(num, name, code, hash, instructions)               \
                                                                    \
    case code: {                                                    \
                                                                    \
        instructions                                                \
                                                                    \
        break;                                                      \
    }                                                                

//===================================================================

int _proc_execute_command(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);
    PROCSTRUCT_IP_CHECK(procstruct);

    unsigned char oper_code = *(procstruct->ip);

    switch (oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"
        #include "../text_files/jumps.txt"

        #undef DEF_CMD_
        #undef DEF_JMP_

        default: 
            error_report(PROC_INV_OPER_CODE);
    }

    #ifdef PROC_DUMPS

        processor_dump(procstruct);

    #endif

    return 0;
}

//===================================================================


int _proc_final_check(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    if (procstruct->ip - procstruct->code_array
        > procstruct->code_file_size
     || procstruct->ip - procstruct->code_array
        < 0)

        error_report(INV_INSTR_PTR);

    return 0;
}

//===================================================================

int _processor_dump(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    fprintf(logs_file, "\n <pre> <div class = \"outline\" style = \"background-color:lightgrey; style = \"text_align: center;>\n");

    fprintf(logs_file, "\n" "Commands executed %d" "\n", 
                                procstruct->command_ct);

    fprintf(logs_file, "\n" "Size of code array: %ld", 
                          procstruct->code_file_size);

    fprintf(logs_file, "\n" "Code array address <%p>" "\n", 
                                   procstruct->code_array);

    if (procstruct->code_file_size != procstruct->header.file_size) {

        fprintf(logs_file, "\n" "ERROR: incorrect code file size:");
    }

    fprintf(logs_file, "\n" "File size written in header"
                                " during compiling: %ld",
                           procstruct->header.file_size);

    fprintf(logs_file, "\n" "Size of file copied during processing: %ld",
                                             procstruct->code_file_size);

    if (procstruct->code_array == NULL) {

        fprintf(logs_file, "\n" "ERROR: invalid pointer to the code array");
        return -1;
    }

    fprintf(logs_file, "\n" "Instruction pointer: %ld" "\n", 
                   procstruct->ip - procstruct->code_array);

    if (procstruct->ip - procstruct->code_array 
                       > procstruct->code_file_size) {

        fprintf(logs_file, "\n" "ERROR: instruction pointer"
                               " points out of code array");
        return -1;
    }

    fprintf(logs_file, "\n" "Code array: " "\n\n");

    int ip_value = (int)(procstruct->ip - procstruct->code_array);
    int val = 0;

    for (int ct = 0; ct < procstruct->code_file_size;) {

        fprintf(logs_file, "%02x ", procstruct->code_array[ct++]);

        if ((ct / 16) * 16 + 16 
                    > (ip_value / 16) * 16 + 16 && val == 0) {

            fprintf(logs_file, "\n" "%*c\n", 
                    (ip_value % 16) * 3 + 1, '^');
            val++;
        }

        else if ((ct % 16) == 0)
            fprintf(logs_file, "\n\n");
    }

    fprintf(logs_file, "\n\n");

    #ifdef DEBUG

        if (stack_validator(&procstruct->stack) == -1) {

            fprintf(logs_file, "\n" "Error in stack work" "\n");
            stack_dump(&procstruct->stack);
        }
        else {

            fprintf(logs_file, "\n" "No errors in stack working" "\n");
            stack_out(&procstruct->stack);
        }

    #else 

        fprintf(logs_file, "\n" "No errors in stack working" "\n");
        stack_out(&procstruct->stack);    

    #endif

    fprintf(logs_file, "\n\n" "Registers: " "\n\n");

    for (int ct = 0; ct < REGISTER_SIZE; ct++) {

        fprintf(logs_file, "[%2c] DEC %7g HEX %7x \n", ct + Ascii_offset + 1,
                                                   procstruct->regist[ct],
                                    (int)procstruct->regist[ct]);
    
        if ( (ct + 1) % 8 == 0)
        fprintf(logs_file, "\n\n");
    }

    for (int counter = 0; counter < 30; counter++)
    {
        fprintf(logs_file, "\n RAM[100 + %d]: %g \n", counter, procstruct->ram[counter + 100]);
    }

    fprintf(logs_file, "\n\n");

    fprintf(logs_file, "\n </div> </pre> \n");

    return 0;
}

//===================================================================


int _video_memory_show_frame(struct Procstruct* procstruct, FILE* output,
                                                              LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    fprintf(output, "\n");                                                     

    for (int counter = 1; counter <= VIDEO_SIZE; counter++) {

        fprintf(output, "%c", (char)(*(procstruct->video + counter - 1)));

        if (counter % (FRAME_WIDTH) == 0)
            fprintf(output, "\n");
    }

    return 0;
}

//===================================================================

char _get_char_from_code(struct Procstruct* procstruct,
                                            LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    char reg_number = (char)(*procstruct->ip - 1U);

    MOVE_IP_TO_NEXT_COMMAND(procstruct);

    if (reg_number > REGISTER_SIZE - 1)
        error_report(INV_REGISTER_NUMBER);

    return reg_number;
}

//===================================================================

elem_t _get_elem_t_from_code(struct Procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    PROCSTRUCT_PTR_CHECK(procstruct);

    elem_t value = *(elem_t*)procstruct->ip;

    procstruct->ip += sizeof(elem_t);
    PROCSTRUCT_IP_CHECK(procstruct);

    return value;
}

//===================================================================

elem_t* _get_pointer(struct Procstruct* procstruct, unsigned char oper_code, 
                                                                 LOG_PARAMS) {
    proc_log_report();

    if (procstruct == NULL)
        return NULL;

    switch(oper_code & ~OPER_CODE_MASK) {

        case (REGISTER_MASK | RAM_MASK): {

            char reg_number = get_char_from_code(procstruct);
            if (reg_number == -1)
                return NULL;

            int ram_number = (int)procstruct->regist[(unsigned char)reg_number];
            if (ram_number < 0 || ram_number > RAM_SIZE - 1)
                return NULL;
            
            elem_t* value_ptr = &procstruct->ram[ram_number];
            
            return value_ptr;
            break;
        }

        case REGISTER_MASK: {

            char reg_number = get_char_from_code(procstruct);
            
            elem_t* value_ptr = &(procstruct->regist[(unsigned char)reg_number]);
            
            return value_ptr;
            break;
        }

        case (IMM_MASK | RAM_MASK): {

            int ram_number = (int)get_elem_t_from_code(procstruct);

            elem_t* value_ptr = &(procstruct->ram[ram_number]);
            
            return value_ptr;
            break;
        }

        case IMM_MASK: {

            elem_t* value_ptr = (elem_t*)procstruct->ip;

            procstruct->ip += sizeof(elem_t);

            if (procstruct->ip - procstruct->code_array
                >procstruct->code_file_size)

                return NULL;

            return value_ptr;
            break;
        }

        case (IMM_MASK | REGISTER_MASK | RAM_MASK): {

            char reg_number = get_char_from_code(procstruct);

            elem_t value = get_elem_t_from_code(procstruct);

            int ram_number = (int)procstruct->regist[reg_number] + (int)value;
            elem_t* value_ptr = &(procstruct->ram[ram_number]);

            return value_ptr;
            break;
        }

        default: {
        
            error_report(PROC_INV_OPER_CODE);
            return NULL;
        }
    }
}
