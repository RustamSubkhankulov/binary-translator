#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "log_definitions.h"

//==================================================================

#define LOG_FILE_DIR_BUF_SIZE 100

#define DIR_NAME_SIZE 300

#define LOG_DIR "/tmp/"

//==================================================================

#define close_log_file() \
       _close_log_file(LOG_ARGS)

//===================================================================

#ifdef CALC_LOGS

    #define error_report(error_code) \
        _error_report(error_code, LOG_ARGS)

#else

    #define error_report(error_code) \
           _error_report(error_code, __FUNCTION__,  __FILE__, __LINE__, logs_file);

#endif

#define log_report() \
       _log_report(LOGS_ARGS_USE, __FUNCTION__)

#define smpl_log_report() \
	   _smpl_log_report(LOG_ARGS)

//===================================================================

int _error_report(int error_code, LOG_PARAMS);

//===================================================================

int dummy_func(void);

FILE* open_log_file(const char* filename);

int write_head_html(FILE* logs_file);

int _close_log_file(LOG_PARAMS);

int _log_report(LOG_PARAMS, const char* mother_func);

int log_report_parameters_check(LOG_PARAMS);

int _smpl_log_report(LOG_PARAMS);

//===================================================================

const char* get_error_descr(int error_code);

//===================================================================

#define $  {                                                        \
                                                                    \
       if (err_val == -1) {                                         \
           fprintf(stderr, "Error occured. "                        \
                           "Process terminating.\n");               \
           return -1;                                               \
       }                                                            \
}

//===================================================================
                                                      
#define OPEN_LOG_FILE(file_ptr, filename) {                         \
                                                                    \
    do                                                              \
    {                                                               \
        file_ptr = open_log_file(filename);                         \
        if (logs_file == NULL)                                      \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}                   

