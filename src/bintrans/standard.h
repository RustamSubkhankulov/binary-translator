#pragma once 

#include <stdlib.h>
#include "../logs/errors_and_logs.h"
#include "standard_conf.h"

//===============================================


#ifdef STANDARD_LOGS

    #define standard_log_report() \
            log_report()

#else 

    #define standard_log_report()

#endif 

//===============================================

float standard_in(void);

int   standard_out(float argument);

//-----------------------------------------------

float standard_sin (float arg);

float standard_cos (float arg);

float standard_tg  (float arg);

float standard_ln  (float arg);

float standard_asin(float arg);

float standard_atg (float arg);

float standard_fabs(float arg);

float standard_pow (float base, float exp);

//-----------------------------------------------

#ifdef FILE_IO

    int _init_file_io(const char* input_name, 
                      const char* output_name
                      FOR_LOGS(, LOG_PARAMS));

    int _close_file_io(FOR_LOGS(LOG_PARAMS));

#endif 

//===============================================

#define init_file_io(in, out) \
       _init_file_io(in, out FOR_LOGS(, LOG_ARGS))

#define close_file_io() \
       _close_file_io(FOR_LOGS(LOG_ARGS))
       