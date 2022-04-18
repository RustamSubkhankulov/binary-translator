#pragma once

#include "../assembler/assembler.h"
#include "../stack/stack.h"
#include "../assembler/config.h"

struct Procstruct {

    long code_file_size;
    const unsigned char* code_array;
    const unsigned char* ip;

    int command_ct;

    elem_t regist[REGISTER_SIZE];

    elem_t* ram;

    elem_t* video;

    struct Header header;

    struct Stack stack;
};

//===================================================================

#ifdef PROC_LOGS

       #define proc_log_report() \
               log_report()

       #define proc_smpl_log_report() \
               smpl_log_report()

#else

       #define proc_log_report() \
               dummy_func()

       #define proc_smpl_log_report() \
               dummy_func()

#endif

//===================================================================

#define init_procstruct(procstruct, fp) \
       _init_procstruct(procstruct, fp, LOG_ARGS)

#define dtor_procstruct(procstruct) \
       _dtor_procstruct(procstruct, LOG_ARGS)

#define processor_dump(procstruct) \
       _processor_dump(procstruct, LOG_ARGS)
//===================================================================

#define read_from_file(fp, size) \
       _read_from_file(fp, size,  LOG_ARGS)

#define header_check(procstruct) \
       _header_check(procstruct, LOG_ARGS)

//===================================================================

#define proc_perform(procstruct) \
       _proc_perform(procstruct, LOG_ARGS)

#define proc_execute_command(procstruct) \
       _proc_execute_command(procstruct, LOG_ARGS)

#define get_char_from_code(procstruct) \
       _get_char_from_code(procstruct, LOG_ARGS)

#define get_elem_t_from_code(procstruct) \
       _get_elem_t_from_code(procstruct, LOG_ARGS)

#define get_pointer(procstruct, oper_code) \
       _get_pointer(procstruct, oper_code, LOG_ARGS)

//===================================================================

#define video_memory_show_frame(procstruct, file_output) \
       _video_memory_show_frame(procstruct, file_output, LOG_ARGS)

#define open_proc_output(filename) \
       _open_proc_output(filename, LOG_ARGS)

#define close_proc_output() \
       _close_proc_output(LOG_ARGS)

#define open_proc_input(filename) \
       _open_proc_input(filename, LOG_ARGS)

#define close_proc_input() \
       _close_proc_input(LOG_ARGS)

//===================================================================

#define proc_final_check(procstruct) \
       _proc_final_check(procstruct, LOG_ARGS)

#define procstruct_allocate_memory(procstruct) \
       _procstruct_allocate_memory(procstruct, LOG_ARGS)

#define procstruct_free_memory(procstruct) \
       _procstruct_free_memory(procstruct, LOG_ARGS)

#define procstruct_ip_check(procstruct) \
       _procstruct_ip_check(procstruct, LOG_ARGS)

//===================================================================

#define GET_ARGUMENT_FROM_STACK(value_pointer) {                      \
                                                                      \
    int err = 0;                                                      \
                                                                      \
    *value_pointer = stack_pop(&procstruct->stack, &err);             \
    if (err == -1)                                                    \
       return -1;                                                     \
}                               

//===================================================================

#define PUSH_ARGUMENT_TO_STACK(value_pointer) {                       \
                                                                      \
    int err = stack_push(&procstruct->stack, *value_pointer);         \
    if (err == -1)                                                    \
       return -1;                                                     \
}

//===================================================================

#define CHECK_DIVIDER_IS_ZERO(value) {                                \
                                                                      \
    if (fabs(value) < PRECISION)                                      \
       error_report(PROC_DIV_BY_ZERO);                         \
}

//===================================================================

#define IS_EQUAL(first, second) (fabs(first - second) < PRECISION)    

//===================================================================

#define PROCSTRUCT_PTR_CHECK(ptr) {                                   \
                                                                      \
    if (ptr == NULL)                                                  \
        error_report(INV_PROCSTRUCT_PTR)                       \
}

//===================================================================

#define PRINT_PROC_OUTPUT(value) {                                    \
                                                                      \
    extern FILE* proc_output;                                         \
                                                                      \
    int fprintf_ret = fprintf(proc_output, ELEM_SPEC_2 "\n", value);  \
                                                                      \
    if (fprintf_ret < 0)                                              \
       error_report(FILE_OUTPUT_ERROR);                        \
}

//===================================================================

#define GET_ARGUMENT_FROM_PROC_INPUT(value_ptr) {                     \
                                                                      \
    extern FILE* proc_input;                                          \
                                                                      \
    int scanf_ret = fscanf(proc_input, ELEM_SPEC, value_ptr);         \
                                                                      \
    if (scanf_ret != 1)                                               \
       error_report(FILE_INPUT_ERR);                           \
}

//===================================================================

#define RAM_NUMBER_CHECK(ram_number) {                                \
                                                                      \
   if (ram_number < 0 || ram_number > RAM_SIZE)                       \
       error_report(INV_RAM_NUMBER);                           \
}

//===================================================================

#define GET_DESTINATION_IP(dest_ip_pointer) {                         \
                                                                      \
       procstruct->ip++;                                              \
       PROCSTRUCT_IP_CHECK(procstruct);                               \
       PROCSTRUCT_PTR_CHECK(procstruct);                              \
                                                                      \
       *dest_ip_pointer = *(int*)procstruct->ip;                      \
    }

//===================================================================

#define MOVE_IP_TO_DEST(dest_ip) {                                    \
                                                                      \
    procstruct->ip = procstruct->code_array + dest_ip;                \
    PROCSTRUCT_IP_CHECK(procstruct);                                  \
}                    

//===================================================================

#define PROCSTRUCT_IP_CHECK(procstruct) {                             \
                                                                      \
    if  (procstruct_ip_check(procstruct) != 1)                        \
        return -1;                                                    \
}

//===================================================================

#define STACK_DESTRUCT(procstruct) {                                  \
                                                                      \
    int err = stack_dtor(&procstruct->stack);                         \
    if (err == -1)                                                    \
        return -1;                                                    \
}

//===================================================================

#define MOVE_IP_TO_NEXT_COMMAND(procstruct) {                         \
                                                                      \
    procstruct->ip++;                                                 \
    PROCSTRUCT_IP_CHECK(procstruct);                                  \
}

//===================================================================

#define PROCESSOR_DUMP(procstruct) {                                  \
                                                                      \
    int is_error = processor_dump(procstruct);                        \
    if (is_error == -1)                                               \
        return -1;                                                    \
}

//===================================================================

#define SHOW_FRAME_TO_CONSOLE_AND_CLEAR(procstruct) {                 \
                                                                      \
    int show_ret = video_memory_show_frame(procstruct, stdout);       \
    if (show_ret == -1)                                               \
        return -1;                                                    \
                                                                      \
    ___System_Sleep(Sleep_duration);                                  \
    system("clear");                                                  \
}

//===================================================================

#define PRINT_FRAME_TO_FILE(procstruct, file_output) {                \
                                                                      \
    int show_ret = video_memory_show_frame(procstruct, file_output);   \
    if (show_ret == -1)                                               \
        return -1;                                                    \
}

//===================================================================

int proc_check_command_line_args(int argc);

int _proc_final_check(struct Procstruct* procstruct, LOG_PARAMS);

int _processor_dump(struct Procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _proc_perform(struct Procstruct* procstruct, LOG_PARAMS);

int _proc_execute_command(struct Procstruct* procstruct, LOG_PARAMS);

char _get_char_from_code(struct Procstruct* procstruct, LOG_PARAMS);

elem_t _get_elem_t_from_code(struct Procstruct* procstruct, LOG_PARAMS);

elem_t* _get_pointer(struct Procstruct* procstruct, unsigned char oper_code, 
                                                                LOG_PARAMS);

int _procstruct_ip_check(struct Procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _init_procstruct(struct Procstruct* procstruct, FILE* fp, LOG_PARAMS);

int _procstruct_allocate_memory(struct Procstruct* procstruct, LOG_PARAMS);

int _procstruct_free_memory(struct Procstruct* procstruct, LOG_PARAMS);

int _dtor_procstruct(struct Procstruct* procstruct, LOG_PARAMS);

//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, LOG_PARAMS);

int _header_check(struct Procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _video_memory_show_frame(struct Procstruct* procstruct, FILE* file_output,
                                                                  LOG_PARAMS);

int _open_proc_output(const char* filename, LOG_PARAMS);

int _close_proc_output(LOG_PARAMS);

int _open_proc_input(const char* filename, LOG_PARAMS);

int _close_proc_input(LOG_PARAMS);

