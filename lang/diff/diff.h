#pragma once

#include "diff_config.h"

#include "../tree/tree.h"
#include "../tree_simp/tree_simp.h"
#include "../logs/errors_and_logs.h"

//-------------------------------------------------------------------

#define NODE_DIFF(src, dest, var) {                                 \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_diff_execute(src, dest, var);                \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}   

#define NODE_COPY(src, dest) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = diff_copy_branch(src, dest);                      \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#ifdef DIFF_LOGS

    #define diff_log_report() \
            log_report()

#else

    #define diff_log_report() ""

#endif

//===================================================================

#ifdef DIFF_TREE_VERIFICATE

    #define DIFF_TREE_VERIFICATION(tree) {                          \
                                                                    \
        do                                                          \
        {                                                           \
            int is_ok = tree_validator(tree);                       \
            if (is_ok == -1)                                        \
                return -1;                                          \
                                                                    \
        } while(0);                                                 \
    }                                           

#else 

    #define DIFF_TREE_VERIFICATION(tree) ""

#endif

//===================================================================

#define BUFFER_STRUCT_PTR_CHECK(buffer_struct_ptr) {                \
                                                                    \
    if (buffer_struct_ptr == NULL) {                                \
                                                                    \
        error_report(BUFFER_STRUCT_INV_PTR);                        \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

#define COMPLEX_DIFF_ADD_SUM(diff_dest, sum_node, diff_tree) {      \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = complex_diff_add_sum(diff_dest, sum_node,         \
                                                diff_tree);         \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    }while(0);                                                      \
}

//===================================================================

// #ifdef DIFF_LATEX

//     #define LATEX(file_ptr) file_ptr

// #else

//     #define LATEX(file_ptr) 

// #endif

//===================================================================

#define DIFF_EXECUTE_SINGLE(tree, diff, tex, var) {                 \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = diff_execute_single(tree, diff, tex, var);        \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define DIFF_EXECUTE_ALL(tree, diff, tex) {                         \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = diff_execute_all(tree, diff, tex);                \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

struct Function {

       int code;
       int64_t hash;
       const char* name;
};

const static struct Function Functions[] {

       { 7563630   , 3141473398, "sin"  }, //hash("sin")
       { 6516595   , 2519927297, "cos"  },
       { 29799     , 1173106379, "tg"   },
       { 6517863   , 3865010701, "ctg"  },
       { 1633906547, 1322787422, "acos" },
       { 1634953582, 2736860584, "asin" },
       { 6386791   , 3546035762, "atg"  },
       { 1633907815, 815273638 , "actg" }
};

const int Functions_number = 8;

//-------------------------------------------------------------------

enum Operands {

       ADD    = '+'        ,
       SUB    = '-'        ,
       MUL    = '*'        ,
       DIV    = '/'        ,
       POW    = '^'        ,
       SIN    = 7563630    ,
       COS    = 6516595    ,
       TG     = 29799      ,
       CTG    = 6517863    ,
       ACOS   = 1633906547 ,
       ASIN   = 1634953582 ,
       ATG    = 6386791    ,
       ACTG   = 1633907815
};

//===================================================================

const int Operands_number = 13;

const int Operands[Operands_number] = {ADD , SUB , MUL, DIV, POW, 
                                       SIN , COS , TG , CTG, 
                                       ACOS, ASIN, ATG, ACTG};

//===================================================================

enum Node_add_parameters {
    LEFT   = 1488,
    RIGHT  = 1489
};

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS);

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS);

int _diff_execute(struct Tree* tree, struct Tree* diff, const char* tex_name, 
                                                                 LOG_PARAMS);

int _node_diff_execute(struct Node* node, struct Node* diff, char var, LOG_PARAMS);

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS);

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS);

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                           int size, int pos, LOG_PARAMS);

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS);

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS);

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS);

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS);

int _diff_copy_branch(struct Node* orig, struct Node* diff, LOG_PARAMS);

int _diff_execute_single(struct Tree* tree, struct Tree* diff, FILE* tex, 
                                                    char var, LOG_PARAMS);

int _diff_execute_all(struct Tree* tree, struct Tree* diff, FILE* tex, 
                                                           LOG_PARAMS);

int diff_read_cmnd_line(int aargc, const char* argv[], const char** logs  , 
                                                       const char** input , 
                                                       const char** output, 
                                                       const char** latex);

//===================================================================

#define diff_tree_ctor(tree) \
       _diff_tree_ctor(tree, LOG_ARGS)

#define diff_tree_dtor(tree) \
       _diff_tree_dtor(tree, LOG_ARGS)

#define diff_read_from_file(tree, filename) \
       _diff_read_from_file(tree, filename, LOG_ARGS)

#define diff_read_from_console(tree) \
       _diff_read_from_console(tree, LOG_ARGS)

#define diff_execute(tree, diff, tex_name) \
       _diff_execute(tree, diff, tex_name, LOG_ARGS)

#define node_diff_execute(orig, diff, var) \
       _node_diff_execute(orig, diff, var, LOG_ARGS)

#define diff_operand_node(orig, diff, var) \
       _diff_operand_node(orig, diff, var, LOG_ARGS)

#define diff_out_to_file(diff, filename) \
       _diff_out_to_file(diff, filename, LOG_ARGS)

#define diff_out_to_console(diff) \
       _diff_out_to_console(diff, LOG_ARGS)

#define buffer_struct_init(buffer_struct, buffer, size, pos) \
       _buffer_struct_init(buffer_struct, buffer, size, pos, LOG_ARGS)

#define buffer_dump(buffer_struct) \
       _buffer_dump(buffer_struct, LOG_ARGS)

#define node_read_from_buffer(node, buffer_struct) \
       _node_read_from_buffer(node, buffer_struct, LOG_ARGS)

#define read_node_data(node, buffer_struct) \
       _read_node_data(node, buffer_struct, LOG_ARGS)

#define read_node_with_children(node, buffer_struct) \
       _read_node_with_children(node, buffer_struct, LOG_ARGS) 

#define read_constant(node, buffer_struct) \
       _read_constant(node, buffer_struct, LOG_ARGS)

#define read_function(node, buffer_struct) \
       _read_function(node, buffer_struct, LOG_ARGS)

#define read_closing_bracket(buffer_struct) \
       _read_closing_bracket(buffer_struct, LOG_ARGS)

#define read_opening_bracket(buffer_struct) \
       _read_opening_bracket(buffer_struct, LOG_ARGS)

#define input_skip_blanks(buffer) \
       _input_skip_blanks(buffer, LOG_ARGS)

#define diff_scan_input() \
       _diff_scan_input(LOG_ARGS)

#define diff_copy_branch(orig, diff) \
       _diff_copy_branch(orig, diff, LOG_ARGS)

#define diff_operand_mul(orig, diff, var) \
       _diff_operand_mul(orig, diff, var, LOG_ARGS)

#define diff_operand_div(orig, diff, var) \
       _diff_operand_div(orig, diff, var, LOG_ARGS)

#define diff_operand_pow(orig, diff, var) \
       _diff_operand_pow(orig, diff, var, LOG_ARGS)

#define diff_operand_add_or_sub(orig, diff, oper, var) \
       _diff_operand_add_or_sub(orig, diff, oper, var, LOG_ARGS)

#define diff_operand_sin(orig, diff, var) \
       _diff_operand_sin(orig, diff, var, LOG_ARGS)

#define diff_operand_cos(orig, diff, var) \
       _diff_operand_cos(orig, diff, var, LOG_ARGS)

#define diff_operand_tg(orig, diff, var) \
       _diff_operand_tg(orig, diff, var, LOG_ARGS)

#define diff_operand_ctg(orig, diff, var) \
       _diff_operand_ctg(orig, diff, var, LOG_ARGS)

#define diff_operand_arcsin_and_arccos(orig, diff, oper, var) \
       _diff_operand_arcsin_and_arccos(orig, diff, oper, var, LOG_ARGS)

#define diff_operand_arctg_and_arcctg(orig, diff, oper, var) \
       _diff_operand_arctg_and_arcctg(orig, diff, oper, var, LOG_ARGS)

#define get_var_diff_by() \
       _get_var_diff_by(LOG_ARGS)

#define complex_diff_add_sum(diff_dest, node_sum, diff) \
       _complex_diff_add_sum(diff_dest, node_sum, diff, LOG_ARGS)

#define node_is_constant(node, var) \
       _node_is_constant(node, var, LOG_ARGS)

#define diff_execute_single(tree, diff, tex, var) \
       _diff_execute_single(tree, diff, tex, var, LOG_ARGS)

#define diff_execute_all(tree, diff, tex) \
       _diff_execute_all(tree, diff, tex, LOG_ARGS)


