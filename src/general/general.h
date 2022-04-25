#pragma once

#include "../logs/errors_and_logs.h"
#include "general_conf.h"

//===================================================================

const double Precise = 0.000001; 

//===================================================================

#ifdef GENERAL_LOGS

    #define general_log_report() \
            log_report()

#else 

    #define general_log_report() 

#endif

//===================================================================

#define ALLOCATION_CHECK(ptr) {                                     \
                                                                    \
    do                                                              \
    {                                                               \
        if (!ptr)                                                   \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define RETURN_CHECK(ret){                                          \
                                                                    \
    do                                                              \
    {                                                               \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define FILENAME_CHECK(filename) {                                  \
                                                                    \
    do                                                              \
    {                                                               \
        if (!filename)                                              \
        {                                                           \
            error_report(INV_FILE_NAME);                            \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define FILE_PTR_CHECK(file) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        if (!file)                                                  \
        {                                                           \
            error_report(INV_FILE_PTR);                             \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//===================================================================

#ifdef ESPEAK

    #define print_and_speak(string) {                                   \
                                                                        \
        printf("\n\n%s\n\n", string);                                   \
                                                                        \
        system("espeak [" #string "]");                                 \
    }

//-------------------------------------------------------------------

    #define fprint_and_speak(output, string) {                          \
                                                                        \
        fprintf(output, "\n\n%s\n\n", string);                          \
                                                                        \
        if (output == stdout)                                           \
            system("espeak [" #string "]");                             \
    }

#else

    #define print_and_speak(string) \
            printf("\n\n%s\n\n", string);

    #define fprint_and_speak(output, string) \
            fprintf(output, "\n\n%s\n\n", string);

#endif

//===================================================================

#define FLOAT_COMPARE(first, second) (abs(first - second) < Precise)

//===================================================================

#define clean_buffer(buffer, size) {                                \
    do                                                              \
    {                                                               \
        memset(buffer, 0, size * sizeof(char));                     \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define get_time_string() \
       _get_time_string(FOR_LOGS(LOG_ARGS))

#define clearstdin() \
       _clearstdin(FOR_LOGS(LOG_ARGS))

#define open_file(filename, mode) \
       _open_file(filename, mode FOR_LOGS(, LOG_ARGS))

#define close_file(file_ptr) \
       _close_file(file_ptr FOR_LOGS(, LOG_ARGS))

#define my_recalloc(ptr, number, prev_number, size_of_elem) \
       _my_recalloc(ptr, number, prev_number, size_of_elem  \
                                     FOR_LOGS(, LOG_ARGS))

#define clear_memory(base, num, size) \
       _clear_memory(base, num, size FOR_LOGS(, LOG_ARGS))

#define is_memory_clear(base, num, size) \
       _is_memory_clear(base, num, size FOR_LOGS(, LOG_ARGS))

#define get_hash(base, len) \
        get_hash_(base, len FOR_LOGS(, LOG_ARGS))

#define get_file_size(src) \
       _get_file_size(src FOR_LOGS(, LOG_ARGS))

#define fast_cpy(dest, src, size) \
       _fast_cpy(dest, src, size FOR_LOGS(, LOG_ARGS))

//===================================================================

int _fast_cpy(void* dest, void* src, unsigned int size FOR_LOGS(, LOG_PARAMS));

char* _get_time_string(FOR_LOGS(LOG_PARAMS));

long _get_file_size(FILE* source FOR_LOGS(, LOG_PARAMS));

int _clearstdin(FOR_LOGS(LOG_PARAMS));

int double_is_equal(double first, double second);

void* _my_recalloc(void* ptr, size_t number, size_t prev_number, size_t size_of_elem
                                                             FOR_LOGS(, LOG_PARAMS));
                                                             
int _clear_memory(void* base, size_t num, size_t size FOR_LOGS(, LOG_PARAMS));

int _is_memory_clear(void* base, size_t num, size_t size FOR_LOGS(, LOG_PARAMS));

void ___System_Sleep(float _Duration);

int my_swap(void* first_, void* second_, size_t size);

int64_t get_hash_(void* void_base, long unsigned int len FOR_LOGS(, LOG_PARAMS));

//===================================================================

FILE* _open_file(const char* filename, const char* mode FOR_LOGS(, LOG_PARAMS));

int _close_file(FILE* file_prt FOR_LOGS(, LOG_PARAMS));