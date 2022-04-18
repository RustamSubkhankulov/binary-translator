#pragma once

#include "syntax.h"

//===================================================================

const int Var_names_start_num   = 5;

const int Label_names_start_num = 5; 

const int Func_names_start_num  = 5;

//===================================================================

#define NAMES_PTR_CHECK(names) {                                    \
                                                                    \
    do                                                              \
    {                                                               \
        if (!names)                                                 \
        {                                                           \
            error_report(INV_NAMES_STRUCT_PTR);                     \
            return NULL;                                            \
        }                                                           \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NAMES_STRUCT_PTR_CHECK(names) {                             \
                                                                    \
    do                                                              \
    {                                                               \
        if (!names)                                                 \
        {                                                           \
            error_report(INV_NAMES_STRUCT_PTR);                     \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}


//-------------------------------------------------------------------

#define VAR_CLASTER_PTR_CHECK(cluster) {                            \
                                                                    \
    do                                                              \
    {                                                               \
        if(!cluster)                                                \
        {                                                           \
            error_report(INV_VAR_CLASTER_PTR);                      \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define LABEL_CLASTER_PTR_CHECK(cluster) {                          \
                                                                    \
    do                                                              \
    {                                                               \
        if(!cluster)                                                \
        {                                                           \
            error_report(INV_LABEL_CLASTER_PTR);                    \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define FUNC_CLASTER_PTR_CHECK(cluster) {                           \
                                                                    \
    do                                                              \
    {                                                               \
        if(!cluster)                                                \
        {                                                           \
            error_report(INV_FUNC_CLASTER_PTR);                     \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ALLOCATION_CHECK(ptr) {                                     \
                                                                    \
    do                                                              \
    {                                                               \
        if (!ptr)                                                   \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//===================================================================

struct Var_name
{
    int64_t id_hash;
    int is_perm;
    int size;
};

struct Var_space
{
    Var_name* var_names;
    int var_names_num;
    int var_names_cap;
};

typedef int64_t Label_name;

struct Func_name
{
    int64_t hash;
    int arg_num;
};

//-------------------------------------------------------------------

struct Var_cluster
{
    Var_space* var_spaces;
    Var_space* cur_var_space;
    int var_spaces_num;
};

struct Label_cluster
{
    Label_name* label_names;
    int label_names_num;
    int label_names_cap;
};

struct Func_cluster
{
    Func_name* func_names;
    int func_names_num;
    int func_names_cap;
};

//-------------------------------------------------------------------

struct Names
{
    Var_cluster*   var_cluster;
    Label_cluster* label_cluster;
    Func_cluster*  func_cluster;
};

//===================================================================

int _init_names_struct (Names* names FOR_LOGS(, LOG_PARAMS));

int _names_struct_dump (Names* names FOR_LOGS(, LOG_PARAMS));

int _kill_names_struct (Names* names FOR_LOGS(, LOG_PARAMS));

int _names_struct_allocate(Names* names FOR_LOGS(, LOG_PARAMS));

int _init_var_cluster  (Var_cluster*   var_cluster   FOR_LOGS(, LOG_PARAMS));

int _kill_var_cluster  (Var_cluster*   var_cluster   FOR_LOGS(, LOG_PARAMS));

int _init_label_cluster(Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS));

int _kill_label_cluster(Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS));

int _init_func_cluster (Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS));

int _kill_func_cluster (Func_cluster*  func_cluster  FOR_LOGS(, LOG_PARAMS));

//-------------------------------------------------------------------

int _add_func_defn   (int64_t hash, int arg_num, Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS));

int _func_is_defined (int64_t hash,              Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS));

int _func_defn_arr_increase                        (Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS));

int _get_func_arg_num(int64_t hash,              Func_cluster* func_cluster FOR_LOGS(, LOG_PARAMS));

//-------------------------------------------------------------------

int _add_var_declare (int64_t hash, int is_perm, int size, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _var_is_declared (int64_t hash, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _var_is_permanent(int64_t hash, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _get_var_size    (int64_t hash, Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _var_decl_arr_increase         (Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _add_var_space                 (Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _rm_var_space                  (Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

int _clear_var_spaces              (Var_cluster* var_cluster FOR_LOGS(, LOG_PARAMS));

//-------------------------------------------------------------------

int _add_label_defn  (int64_t hash, Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS));

int _label_is_defined(int64_t hash, Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS));

int _label_defn_arr_increase       (Label_cluster* label_cluster FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define names_struct_allocate(names) \
       _names_struct_allocate(names FOR_LOGS(, LOG_ARGS))

#define init_names_struct(names) \
       _init_names_struct(names FOR_LOGS(, LOG_ARGS))

#define names_struct_dump(names) \
       _names_struct_dump(names FOR_LOGS(, LOG_ARGS))

#define kill_names_struct(names) \
       _kill_names_struct(names FOR_LOGS(, LOG_ARGS))

#define init_var_cluster(cluster) \
       _init_var_cluster(cluster FOR_LOGS(, LOG_ARGS))

#define init_func_cluster(cluster) \
       _init_func_cluster(cluster FOR_LOGS(, LOG_ARGS))

#define init_label_cluster(cluster) \
       _init_label_cluster(cluster FOR_LOGS(, LOG_ARGS))

#define kill_var_cluster(cluster) \
       _kill_var_cluster(cluster FOR_LOGS(, LOG_ARGS))

#define kill_func_cluster(cluster) \
       _kill_func_cluster(cluster FOR_LOGS(, LOG_ARGS))

#define kill_label_cluster(cluster) \
       _kill_label_cluster(cluster FOR_LOGS(, LOG_ARGS))  

//------------------------------------------------------------------- 

#define add_func_defn(hash, arg_num, cluster) \
       _add_func_defn(hash, arg_num, cluster FOR_LOGS(, LOG_ARGS))

#define func_is_defined(hash, cluster) \
       _func_is_defined(hash, cluster FOR_LOGS(, LOG_ARGS))

#define func_defn_arr_increase(cluster) \
       _func_defn_arr_increase(cluster FOR_LOGS(, LOG_ARGS))

#define get_func_arg_num(hash, cluster) \
       _get_func_arg_num(hash, cluster FOR_LOGS(,LOG_ARGS))

//-------------------------------------------------------------------

#define add_label_defn(hash, cluster) \
       _add_label_defn(hash, cluster FOR_LOGS(, LOG_ARGS))

#define label_is_defined(hash, cluster) \
       _label_is_defined(hash, cluster FOR_LOGS(, LOG_ARGS))

#define label_defn_arr_increase(cluster) \
       _label_defn_arr_increase(cluster FOR_LOGS(, LOG_ARGS))


//-------------------------------------------------------------------

#define add_var_declare(hash, is_perm, size, cluster) \
       _add_var_declare(hash, is_perm, size, cluster FOR_LOGS(, LOG_ARGS))

#define var_is_declared(hash, cluster) \
       _var_is_declared(hash, cluster FOR_LOGS(, LOG_ARGS))

#define var_is_permanent(hash, cluster) \
       _var_is_permanent(hash, cluster FOR_LOGS(, LOG_ARGS))

#define get_var_size(hash, cluster) \
       _get_var_size(hash, cluster FOR_LOGS(, LOG_ARGS))

#define var_decl_arr_increase(cluster) \
       _var_decl_arr_increase(cluster FOR_LOGS(, LOG_ARGS))

#define add_var_space(cluster) \
       _add_var_space(cluster FOR_LOGS(, LOG_ARGS))

#define rm_var_space(cluster) \
       _rm_var_space(cluster FOR_LOGS(, LOG_ARGS))

#define clear_var_spaces(cluster) \
       _clear_var_spaces(cluster FOR_LOGS(, LOG_ARGS))

//===================================================================
