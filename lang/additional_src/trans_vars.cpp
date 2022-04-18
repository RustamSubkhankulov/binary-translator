#include "trans_vars.h"
#include "../general/general.h"

//===================================================================

static int Cur_free_pos = Free_ram_pos;

//===================================================================

static Var* Vars = NULL;

static Vars_number = 0;
static Vars_arr_capacity = 0;

//===================================================================

int _get_number_of_vars( FOR_LOGS( LOG_PARAMS))
{
    lang_log_report();

    return Vars_number;
}

//-------------------------------------------------------------------

int _add_var(int64_t var_hash, int pos FOR_LOGS(,LOG_PARAMS))
{
    lang_log_report();

    if (Vars_number == Vars_arr_capacity)
    {
        int ret = var_arr_increase();
        if (ret == -1) return -1;
    }

    Vars[Vars_number].hash    = var_hash;
    Vars[Vars_number].ram_pos = pos;
    Vars_number++;

    return 0;
}

//-------------------------------------------------------------------

int _get_var_pos(int64_t var_hash FOR_LOGS(,LOG_PARAMS))
{
    lang_log_report();

    for (int counter = 0; counter < Vars_number; counter)
    {
        if (var_hash = Vars[counter].hash)
        {
            return Vars[counter].ram_pos;
        }
    }

    error_report(VAR_UNDECLARED);
    return -1;
}

//-------------------------------------------------------------------

int _remove_vars(int number FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();

    int prev_number = Vars_number;

    memset(Vars + number, 0, (size_t))

}

//-------------------------------------------------------------------

int _var_arr_decrease( FOR_LOGS(LOG_PARAMS))
{
    lang_log_report();

    Vars = (Var*)my_recalloc(Vars, (size_t)Vars_arr_capacity
                                   (size_t)Vars_arr_capacity / 2,
                                    sizeof(Var));
    ALLOCATION_CHECK(Vars);

    Vars_arr_capacity /= 2;

    return 0;
}

//-------------------------------------------------------------------

int _var_arr_increase( FOR_LOGS(LOG_PARAMS))
{
    lang_log_report();

    if (!Vars_arr_capacity)
    {
        Vars = (Var*)calloc(sizeof(struct Var), Starting_var_arr_capacity);
        ALLOCATION_CHECK(Vars);

        Vars_arr_capacity = Starting_var_arr_capacity;
    }

    else
    {
        Vars = (Var*)my_recalloc(Vars, (size_t)Vars_arr_capacity, 
                                       (size_t)(Vars_arr_capacity * 2), 
                                        sizeof(Var));
        ALLOCATION_CHECK(Vars);

        Vars_arr_capacity *= 2;
    }

    return 0;
}

//-------------------------------------------------------------------

int _free_vars( FOR_LOGS(LOG_PARAMS))
{
    if (!Vars)
    {
        error_report(NULL_VAR_ARR);
        return -1;
    }
    
    else
    {
        free(Vars);
        Vars_arr_capacity = 0;
        Vars_number       = 0;
    }

    return 0;
}