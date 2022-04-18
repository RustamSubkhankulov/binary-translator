#include <stdlib.h>

#include "syntax.h"
#include "../general/general.h"

//===================================================================

int _init_names_struct(Names* names FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NAMES_STRUCT_PTR_CHECK(names);

    int ret = 0;

    ret = names_struct_allocate(names);
    RETURN_CHECK(ret);

    ret = init_var_cluster(names->var_cluster);
    RETURN_CHECK(ret);

    ret = init_label_cluster(names->label_cluster);
    RETURN_CHECK(ret);

    ret = init_func_cluster(names->func_cluster);
    RETURN_CHECK(ret);

    return 0;
}

//-------------------------------------------------------------------

int _names_struct_allocate(Names* names FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NAMES_STRUCT_PTR_CHECK(names);

    names->var_cluster   = (Var_cluster*)  calloc(sizeof(Var_cluster),   sizeof(char));
    if (!names->var_cluster)
        return -1;

    names->func_cluster  = (Func_cluster*) calloc(sizeof(Func_cluster),  sizeof(char));
    if (!names->func_cluster)
        return -1;

    names->label_cluster = (Label_cluster*)calloc(sizeof(Label_cluster), sizeof(char));
    if (!names->label_cluster)
        return -1;

    return 0;
}

//-------------------------------------------------------------------

int _kill_names_struct(Names* names FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NAMES_STRUCT_PTR_CHECK(names);

    int ret = 0;

    ret = kill_var_cluster(names->var_cluster);
    RETURN_CHECK(ret);

    ret = kill_label_cluster(names->label_cluster);
    RETURN_CHECK(ret);

    ret = kill_func_cluster(names->func_cluster);
    RETURN_CHECK(ret);

    if (names->var_cluster)
        free(names->var_cluster);
    else
    {
        error_report(INV_VAR_CLASTER_PTR);
        return -1;
    }

    if (names->label_cluster)
        free(names->label_cluster);
    else
    {
        error_report(INV_LABEL_CLASTER_PTR);
        return -1;
    }

    if (names->func_cluster)
        free(names->func_cluster);
    else
    {
        error_report(INV_FUNC_CLASTER_PTR);
        return -1;
    }
    
    return 0;
}

//-------------------------------------------------------------------

int _init_var_cluster  (Var_cluster*   var_cluster   FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    var_cluster->var_spaces = (Var_space*)calloc(sizeof(Var_space), sizeof(char));
    if (!var_cluster->var_spaces)
        return -1;

    var_cluster->var_spaces_num = 1;
    var_cluster->cur_var_space  = var_cluster->var_spaces;

    var_cluster->cur_var_space->var_names = (Var_name*)calloc(Var_names_start_num, sizeof(Var_name));
    if (!var_cluster->cur_var_space->var_names)
        return -1;

    var_cluster->cur_var_space->var_names_cap = Var_names_start_num;

    return 0;
}

//-------------------------------------------------------------------

int _kill_var_cluster  (Var_cluster*   var_cluster   FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)
    {
        if (var_cluster->var_spaces[counter].var_names)
            free(var_cluster->var_spaces[counter].var_names);
    }

    if (var_cluster->var_spaces)
        free(var_cluster->var_spaces);

    var_cluster->var_spaces     = NULL;
    var_cluster->cur_var_space  = NULL;
    var_cluster->var_spaces_num = 0;

    return 0;
}

//-------------------------------------------------------------------

int _init_label_cluster(Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    LABEL_CLASTER_PTR_CHECK(label_cluster);

    label_cluster->label_names = (Label_name*)calloc(Label_names_start_num, sizeof(Label_name));
    if (!label_cluster->label_names)
        return -1;

    label_cluster->label_names_cap = Label_names_start_num;

    return 0;
}

//-------------------------------------------------------------------

int _kill_label_cluster(Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    LABEL_CLASTER_PTR_CHECK(label_cluster);

    if (label_cluster->label_names)
        free(label_cluster->label_names);

    label_cluster->label_names = NULL;
    label_cluster->label_names_cap = 0;
    label_cluster->label_names_num = 0;

    return 0;
}

//-------------------------------------------------------------------

int _init_func_cluster (Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FUNC_CLASTER_PTR_CHECK(func_cluster);

    func_cluster->func_names = (Func_name*)calloc(Func_names_start_num, sizeof(Func_name));
    if (!func_cluster->func_names)
        return -1;

    func_cluster->func_names_cap = Func_names_start_num;

    return 0;
}

//-------------------------------------------------------------------

int _kill_func_cluster (Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS))
{
    if (func_cluster->func_names)
        free(func_cluster->func_names);

    func_cluster->func_names = NULL;
    func_cluster->func_names_cap = 0;
    func_cluster->func_names_num = 0;

    return 0;
}

//-------------------------------------------------------------------

int _add_func_defn(int64_t hash, int arg_num, Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FUNC_CLASTER_PTR_CHECK(func_cluster);

    if (func_cluster->func_names_num == func_cluster->func_names_cap - 1)
    {
        int ret = func_defn_arr_increase(func_cluster);
        RETURN_CHECK(ret);
    }

    func_cluster->func_names[func_cluster->func_names_num].hash    = hash;
    func_cluster->func_names[func_cluster->func_names_num].arg_num = arg_num; 
    func_cluster->func_names_num++;

    return 0;
}

//-------------------------------------------------------------------

int _func_is_defined (int64_t hash, Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FUNC_CLASTER_PTR_CHECK(func_cluster);

    for (int counter = 0; counter < func_cluster->func_names_num; counter++)
    {
        if (hash == func_cluster->func_names[counter].hash)
            return 1;
    }

    return 0;
}


//-------------------------------------------------------------------

int _func_defn_arr_increase(Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FUNC_CLASTER_PTR_CHECK(func_cluster);

    func_cluster->func_names = (Func_name*)my_recalloc(func_cluster->func_names, 
                                              (size_t)(func_cluster->func_names_cap * 2), 
                                               size_t (func_cluster->func_names_cap), 
                                               sizeof(Func_name));
    if (!func_cluster->func_names)
        return -1;

    func_cluster->func_names_cap *= 2;

    return 0;
}

//-------------------------------------------------------------------

int _get_func_arg_num(int64_t hash, Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FUNC_CLASTER_PTR_CHECK(func_cluster);

    for (int counter = 0; counter < func_cluster->func_names_num; counter++)
    {
        if (hash == func_cluster->func_names[counter].hash)
            return func_cluster->func_names[counter].arg_num;
    }

    error_report(FUNC_UNDECLARED);
    return -1;
}

//-------------------------------------------------------------------

int _add_var_declare(int64_t hash, int is_perm, int size, Var_cluster* var_cluster 
                                                           FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    if (var_cluster->cur_var_space->var_names_num 
    ==  var_cluster->cur_var_space->var_names_cap - 1)
    {
        int ret = var_decl_arr_increase(var_cluster);
        RETURN_CHECK(ret);
    }

    Var_name* var_names     = var_cluster->cur_var_space->var_names;
    int       var_names_num = var_cluster->cur_var_space->var_names_num;

    var_names[var_names_num].id_hash = hash;
    var_names[var_names_num].is_perm = is_perm;
    var_names[var_names_num].size    = size;

    var_cluster->cur_var_space->var_names_num++;

    return 0;
}

//-------------------------------------------------------------------

int _var_is_declared (int64_t hash, Var_cluster*   var_cluster   FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)

        for (int ct = 0; ct < var_cluster->var_spaces[counter].var_names_num; ct++)
        {
            if (hash == var_cluster->var_spaces[counter].var_names[ct].id_hash)
                return 1;
        }

    return 0;
}

//------------------------------------------------------------------

int _var_is_permanent(int64_t hash, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)

        for (int ct = 0; ct < var_cluster->var_spaces[counter].var_names_num; ct++)
        {
            if (hash == var_cluster->var_spaces[counter].var_names[ct].id_hash)
                return  var_cluster->var_spaces[counter].var_names[ct].is_perm;
        }

    error_report(VAR_UNDECLARED);
    return -1;
}

//-------------------------------------------------------------------

int _get_var_size(int64_t hash, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)

        for (int ct = 0; ct < var_cluster->var_spaces[counter].var_names_num; ct++)
        {
            if (hash == var_cluster->var_spaces[counter].var_names[ct].id_hash)
                return  var_cluster->var_spaces[counter].var_names[ct].size;
        }

    error_report(VAR_UNDECLARED);
    return -1;
}

//-------------------------------------------------------------------

int _var_decl_arr_increase(Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    int var_names_cap   = var_cluster->cur_var_space->var_names_cap;
    Var_name* var_names = var_cluster->cur_var_space->var_names;

    var_cluster->cur_var_space->var_names = (Var_name*)my_recalloc(var_names, 
                                                          (size_t)(var_names_cap * 2), 
                                                          (size_t) var_names_cap, 
                                                           sizeof (Var_name));
    if (!var_cluster->cur_var_space->var_names)
        return -1;

    var_cluster->cur_var_space->var_names_cap *= 2;

    return 0;
}

//-------------------------------------------------------------------

int _add_var_space(Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    int var_spaces_num = var_cluster->var_spaces_num;

    if (!var_spaces_num)
    {
        var_cluster->var_spaces = (Var_space*)calloc(sizeof(Var_space), sizeof(char));
        if (!var_cluster->var_spaces)
            return -1;
        
        var_cluster->var_spaces_num = 1;
        var_cluster->cur_var_space = var_cluster->var_spaces;
    }

    else
    {
        var_cluster->var_spaces = (Var_space*)my_recalloc(var_cluster->var_spaces, 
                                                              (size_t)(var_spaces_num + 1), 
                                                              (size_t) var_spaces_num, 
                                                               sizeof (Var_space));
        if (!var_cluster->var_spaces)
            return -1;

        var_cluster->var_spaces_num++;
        var_cluster->cur_var_space = &(var_cluster->var_spaces[var_cluster->var_spaces_num - 1]);
    }
    var_cluster->cur_var_space->var_names = (Var_name*)calloc(Var_names_start_num, sizeof(Var_name));
    if (!var_cluster->cur_var_space->var_names)
        return -1;

    var_cluster->cur_var_space->var_names_cap = Var_names_start_num;

    return 0;
}

//-------------------------------------------------------------------

int _clear_var_spaces(Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)
    {
        if (var_cluster->var_spaces[counter].var_names)
            free(var_cluster->var_spaces[counter].var_names);
    }

    free(var_cluster->var_spaces);
    var_cluster->var_spaces    = NULL;
    var_cluster->cur_var_space = NULL;
    var_cluster->var_spaces_num = 0;

    return 0;    
}

//-------------------------------------------------------------------

int _rm_var_space(Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    VAR_CLASTER_PTR_CHECK(var_cluster);

    if (var_cluster->cur_var_space->var_names)
        free(var_cluster->cur_var_space->var_names);

    int var_spaces_num = var_cluster->var_spaces_num;

    if (var_spaces_num == 1)
    {
        free(var_cluster->var_spaces);

        var_cluster->var_spaces_num = 0;
        var_cluster->var_spaces     = NULL;
        var_cluster->cur_var_space  = NULL;
    }

    else
    {
        var_cluster->var_spaces = (Var_space*)my_recalloc(var_cluster->var_spaces, 
                                                              (size_t)(var_spaces_num - 1), 
                                                              (size_t) var_spaces_num, 
                                                               sizeof (Var_space));
        if (!var_cluster->var_spaces)
            return -1;

        var_cluster->var_spaces_num--;
        var_cluster->cur_var_space = &(var_cluster->var_spaces[var_cluster->var_spaces_num - 1]);
    }

    return 0;
}

//-------------------------------------------------------------------

int _add_label_defn  (int64_t hash, Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    LABEL_CLASTER_PTR_CHECK(label_cluster);

    if (label_cluster->label_names_num == label_cluster->label_names_cap - 1)
    {
        int ret = label_defn_arr_increase(label_cluster);
        RETURN_CHECK(ret);
    }

    label_cluster->label_names[label_cluster->label_names_num++] = hash;
    return 0;
}

//-------------------------------------------------------------------

int _label_is_defined(int64_t hash, Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    LABEL_CLASTER_PTR_CHECK(label_cluster);

    for (int counter = 0; counter < label_cluster->label_names_num; counter++)
    {
        if (hash == label_cluster->label_names[counter])
            return 1;
    }

    return 0;
}

//-------------------------------------------------------------------

int _label_defn_arr_increase(Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    LABEL_CLASTER_PTR_CHECK(label_cluster);

    label_cluster->label_names = (Label_name*)my_recalloc(label_cluster->label_names, 
                                                 (size_t)(label_cluster->label_names_cap * 2), 
                                                 (size_t) label_cluster->label_names_cap, 
                                                  sizeof(Label_name));
    if (!label_cluster->label_names)
        return -1;

    label_cluster->label_names_cap *= 2;

    return 0;
}

//--------------------------------------------------------------------

int _names_struct_dump(Names* names FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NAMES_STRUCT_PTR_CHECK(names);

    fprintf(logs_file, "<pre><div class=\"outline\"  "
                                   "style = \"background-color:lightgrey;\" "
                                   "style = \"text-align: center;\">\n");

    fprintf(logs_file, "\n\n <b> Names structure dump </b> \n");

    fprintf(logs_file, "Address: <%p>\n", names);

    Var_cluster* var_cluster = names->var_cluster;
    fprintf(logs_file, "\n <b> Var cluster </b> <%p> \n", names->var_cluster);
    fprintf(logs_file, "Var_spaces: <%p> Current var_space: <%p> Var spaces number: %d\n", var_cluster->var_spaces, 
                                                                                           var_cluster->cur_var_space, 
                                                                                           var_cluster->var_spaces_num);

    for (int counter = 0; counter < var_cluster->var_spaces_num; counter++)
    {
        fprintf(logs_file, "\n Var space [%d] <%p> : Number of names: %d Capacity: %d\n", counter, var_cluster->var_spaces + counter, 
                                                                                                   var_cluster->var_spaces[counter].var_names_num, 
                                                                                                   var_cluster->var_spaces[counter].var_names_cap);
        for (int ct = 0; ct < var_cluster->var_spaces[counter].var_names_num; ct++)
            fprintf(logs_file, "\n [%d] Name hash %ld Is permanent %d Size %d \n", ct, var_cluster->var_spaces[counter].var_names[ct].id_hash, 
                                                                                       var_cluster->var_spaces[counter].var_names[ct].is_perm,
                                                                                       var_cluster->var_spaces[counter].var_names[ct].size);
    }

    Label_cluster* label_cluster = names->label_cluster;

    fprintf(logs_file, "\n <b> Labels cluster: </b> <%p> \n", label_cluster);
    fprintf(logs_file, " Number of labels: %d Labels array capacity: %d \n", label_cluster->label_names_num, 
                                                                             label_cluster->label_names_cap);

    for (int counter = 0; counter < label_cluster->label_names_num; counter++)
    {
        fprintf(logs_file, "\n [%d] Label name hash: %ld \n", counter, label_cluster->label_names[counter]);
    }

    Func_cluster* func_cluster = names->func_cluster;

    fprintf(logs_file, "\n <b> Func cluster: </b> <%p> \n", func_cluster);
    fprintf(logs_file, " Number of functions: %d Func array capacity: %d \n", func_cluster->func_names_num, 
                                                                              func_cluster->func_names_cap);

    for (int counter = 0; counter < func_cluster->func_names_num; counter++)
    {
        fprintf(logs_file, "\n [%d] Func name hash: %ld Arguments number: %d\n", counter, func_cluster->func_names[counter].hash,
                                                                                          func_cluster->func_names[counter].arg_num);
    }

    fprintf(logs_file, "\n </div></pre> \n");

    return 0;
}

//===================================================================