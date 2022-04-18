#include "diff_variables.h"
#include "../general/general.h"

//===================================================================

char Variables[Variables_max_number] = { 0 };

int Variables_counter = 0;

//===================================================================

int _check_new_var(char variable, LOG_PARAMS) {

    diff_log_report();

    for (int counter = 0; counter < Variables_counter; counter++) 

        if (Variables[counter] == variable)
            return 0;

    return 1;
}

//===================================================================

int _add_new_var(char variable, LOG_PARAMS) {

    diff_log_report();

    Variables[Variables_counter++] = variable;

    return 0;
}

//===================================================================

int _print_vars(FILE* output, LOG_PARAMS) {

    diff_log_report();

    if (!output) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    for (int counter = 0; counter < Variables_counter; counter++)

        fprintf(output, " %d). %c ", counter + 1, Variables[counter]);

    return 0;
}

//===================================================================

char _get_var_by_number(int number, LOG_PARAMS) {

    return Variables[number];
}

//===================================================================

int _get_vars_number(LOG_PARAMS) {

    return Variables_counter;
}

