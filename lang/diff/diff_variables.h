#include "diff.h"

//===================================================================

int _check_new_var  (char variable, LOG_PARAMS);

int _add_new_var    (char variable, LOG_PARAMS);

int _print_vars      (FILE* output, LOG_PARAMS);

char _get_var_by_number(int number, LOG_PARAMS);

int _get_vars_number               (LOG_PARAMS);
 
//===================================================================

#define check_new_var(var) \
       _check_new_var(var, LOG_ARGS)

#define add_new_var(var) \
       _add_new_var(var, LOG_ARGS)

#define print_vars(output) \
       _print_vars(output, LOG_ARGS)

#define get_var_by_number(number) \
       _get_var_by_number(number, LOG_ARGS)

#define get_vars_number() \
       _get_vars_number(LOG_ARGS)




