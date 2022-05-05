#pragma once 

#include "optimizer_conf.h"
#include "bintrans.h"

//===============================================

int _binary_optimize(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

//===============================================

#define binary_optimize(trans_struct) \
       _binary_optimize(trans_struct FOR_LOGS(, LOG_ARGS))

       