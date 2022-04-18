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

int _binary_translate(FOR_LOGS(LOG_PARAMS));

//===============================================

#define binary_translate() \
       _binary_translate( FOR_LOGS(LOG_ARGS))
