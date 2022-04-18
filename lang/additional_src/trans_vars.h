#pragma once

#include "trans.h"

//===================================================================

struct Var
{
    int64_t hash;
    int ram_pos;
};

//===================================================================

int _get_number_of_vars( FOR_LOGS( LOG_PARAMS));

int _add_var(int64_t var_hash, int pos FOR_LOGS(, LOG_PARAMS));

int _get_var_pos(int64_t var_hash FOR_LOGS(,LOG_PARAMS));

int _remove_vars(int prev_number FOR_LOGS(, LOG_PARAMS));

int _var_arr_decrease( FOR_LOGS(LOG_PARAMS));

int _var_arr_increase( FOR_LOGS(LOG_PARAMS));

int _free_vars( FOR_LOGS(LOG_PARAMS));

//===================================================================

#define get_number_of_vars() \
       _get_number_of_vars( FOR_LOGS (LOG_ARRGS))

#define add_var(hash, pos) \
       _add_var(hash, pos FOR_LOGS(, LOG_ARGS))

#define get_var_pos(hash) \
       _get_var_pos(hash FOR_LOGS(, LOG_ARGS))

#define remove_vars(prev_number) \
       _remove_vars(prev_number FOR_LOGS(, LOG_ARGS))

#define var_arr_increase() \
       _var_arr_increase( FOR_LOGS(LOG_ARGS))

#define var_arr_decrease() \
       _var_arr_decrease( FOR_LOGS(LOG_ARGS))

#define free_vars() \
       _free_vars( FOR_LOGS(LOG_ARGS))