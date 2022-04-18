#pragma once

#include "tree_config.h"
#include "../logs/errors_and_logs.h"
#include "../lexic/lexic.h"

//===================================================================

#define CONNECT_RIGHT(parent, son) {                                \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = nodes_connect_right(parent, son);                 \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//------------------------------------------------------------------

#define CONNECT_LEFT(parent, son) {                                 \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = nodes_connect_left(parent, son);                  \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define ADD_POINTERS(logs, input, output, latex)                    \
                                                                    \
    const char* logs   = NULL;                                      \
    const char* input  = NULL;                                      \
    const char* output = NULL;                                      \
    const char* latex  = NULL;                                      \

//===================================================================

#define NODE_INIT_CONSTANT(node, value) {                           \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_constant(node, value);                  \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

#define ADD_CONSTANT_NODE(node, value) {                            \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_constant(node, value);                  \
        if (ret == -1)                                              \
            return -1;                                              \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_INIT_IDENTIFICATOR(node, id_hash) {                    \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_identificator(node, id_hash);           \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_INIT_BIN_OPERAND(node, oper) {                         \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_bin_operand(node, oper);                \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//------------------------------------------------------------------

#define NODE_INIT_KEY_NODE(node, code) {                            \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_key_node(node, code);                   \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//------------------------------------------------------------------

#define NODE_INIT_STD_FUNC_NODE(node, code) {                       \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_std_func_node(node, code);              \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_INIT_UNAR_OPERAND(node, oper) {                        \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_unar_operand(node, oper);               \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_LEFT(node) {                                            \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_left_son(node);                          \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_RIGHT(node) {                                           \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_right_son(node);                         \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_LEFT_AND_RIGHT(node) {                                  \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_sons(node);                              \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_CONSTANT(node, son, constant) {                         \
                                                                    \
    do                                                              \
    {                                                               \
        switch(son) {                                               \
                                                                    \
            case LEFT: {                                            \
                                                                    \
                int ret = node_add_left_son(node);                  \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_constant(node->left_son, constant); \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            case RIGHT: {                                           \
                                                                    \
                int ret = node_add_right_son(node);                 \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_constant(node->right_son, constant);\
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            default: {                                              \
                                                                    \
                error_report(INV_ADD_PARAMETER);                    \
                return -1;                                          \
            }                                                       \
        }                                                           \
    } while(0);                                                     \
}

//===================================================================

struct Node * const No_parent = (struct Node*)993;

//===================================================================

union Data 
{
    double  constant;
    int     key_node_code;
    int     std_func_code;
    int     operand_code;
    int64_t id_hash;
};

//===================================================================

struct Node 
{
    union  Data  data;

    struct Node* parent;

    struct Node* left_son;
    struct Node* right_son;

    int data_type;
};

//===================================================================

enum key_node_codes
{
    STATEMENT_ND = 1,
    LABEL_ND     = 2,
    GOTO_ND      = 3,
    RET_ND       = 4,
    WHILE_ND     = 5,
    CALL_ND      = 6,
    FUNC_ND      = 7,
    DEFN_ND      = 8,
    PARAMETER_ND = 9,
    ASS_ND       = 10,
    DECL_ND      = 11,
    HEAD_ND      = 12,
    DECISION_ND  = 13,
    IF_ND        = 14,
    FUNC_DEFN_ND = 15,
    PERM_ND      = 16,
    SIZE_ND      = 17,
    INDEX_ND     = 18,
    ELEM_ND      = 19
};

struct Key_node_info
{
    int key_node_code;
    const char* name;
};

const struct Key_node_info Key_node_info_table[] = 
{
    {STATEMENT_ND , "STAT"      },
    {LABEL_ND     , "LABEL"     },
    {GOTO_ND      , "GOTO"      },
    {RET_ND       , "RETURN"    },
    {WHILE_ND     , "WHILE"     },
    {CALL_ND      , "CALL"      },
    {FUNC_ND      , "FUNC"      },
    {DEFN_ND      , "DEFN"      },
    {FUNC_DEFN_ND , "FUNC DEFN" },
    {PARAMETER_ND , "PARAM"     },
    {ASS_ND       , "ASS"       },
    {DECL_ND      , "DECL"      },
    {HEAD_ND      , "HEAD"      },
    {DECISION_ND  , "DESIC"     },
    {IF_ND        , "IF"        },
    {PERM_ND      , "PERMANENT" },
    {SIZE_ND      , "SIZE"      },
    {INDEX_ND     , "INDEX"     },
    {ELEM_ND      , "ELEMENT"   }
};

const int Key_node_number = sizeof(Key_node_info_table) / sizeof(struct Key_node_info);

//===================================================================

enum data_types 
{       
    CONSTANT          = 111,
    IDENTIFICATOR     = 222,
    BIN_OPERAND       = 333,    //+cmp signs
    UNAR_OPERAND      = 444,
    KEY_NODE          = 555,
    STD_FUNC_NODE     = 666
};

//===================================================================

struct Tree 
{
    struct Node* root;
};

//===================================================================

struct Node_dot 
{
    FILE* graph;
    int father;

    Node* root;
};

//===================================================================

#ifdef TREE_LOGS

    #define tree_log_report() \
            log_report()

#else 

    #define tree_log_report() \
            dummy_func()

#endif

//===================================================================

#ifndef TREE_VERIFICATE

    #define TREE_VERIFICATION(tree) ""

#else

    #define TREE_VERIFICATION(tree) {                               \
                                                                    \
        do                                                          \
        {                                                           \
            int is_ok = tree_validator(tree);                       \
            if (is_ok == -1)                                        \
                return -1;                                          \
                                                                    \
        } while(0);                                                 \
    }

#endif

//===================================================================

#define TREE_PTR_CHECK(tree_ptr) {                                  \
                                                                    \
    if (tree_ptr == NULL) {                                         \
                                                                    \
        error_report(INV_TREE_PTR);                                 \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

#define NODE_PTR_CHECK(node_ptr) {                                  \
                                                                    \
    if (node_ptr == NULL) {                                         \
                                                                    \
        error_report(INV_NODE_PTR);                                 \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

#define NODE_DOT_PTR_CHECK(node_dot_ptr) {                          \
                                                                    \
    if (node_dot_ptr == NULL) {                                     \
                                                                    \
        error_report(INV_NODE_DOT_PTR);                             \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

int _tree_save_to_file  (struct Tree* tree, const char* filename, LOG_PARAMS);

int _node_save_to_file  (struct Node* node, FILE* output, LOG_PARAMS);

int _node_dot_fill      (struct Node_dot* node_dot, FILE* graph_output, 
                                         int father, LOG_PARAMS);

int _tree_clear_check   (struct Tree* tree, LOG_PARAMS);

int _tree_poison_check  (struct Tree* tree, LOG_PARAMS);

int _tree_ctor          (struct Tree* tree, LOG_PARAMS);

int _tree_dtor          (struct Tree* tree, LOG_PARAMS);

int _node_visiter       (struct Node* node, int (*node_function)(struct Node* node, LOG_PARAMS), 
                                                                                   LOG_PARAMS);

int _tree_validator     (struct Tree* tree, LOG_PARAMS);

int _node_validator     (struct Node* node, LOG_PARAMS);

int _node_destruct      (struct Node* node, LOG_PARAMS);

int _node_add_right_son (struct Node* node, LOG_PARAMS);

int _node_add_left_son  (struct Node* node, LOG_PARAMS);

int _node_add_sons      (struct Node* node, LOG_PARAMS);

void* _node_allocate_memory(LOG_PARAMS);

// int _node_init(struct Node* node, elem_t value, LOG_PARAMS);

int _tree_free_memory   (struct Tree* tree, LOG_PARAMS);

int _node_print         (struct Node* node, LOG_PARAMS);

int _tree_dump          (struct Tree* tree, LOG_PARAMS);

int _tree_print         (struct Tree* tree, LOG_PARAMS);

int _tree_delete_branch (struct Node** node_ptr, LOG_PARAMS);

int _node_delete_branch (struct Node* node, LOG_PARAMS);

int _tree_draw_graph    (struct Tree* tree, LOG_PARAMS);

int _node_draw_graph    (struct Node* node, struct Node_dot* node_dot,  LOG_PARAMS);
 
int _tree_poisoning     (struct Tree* tree, LOG_PARAMS);

int _tree_cleaning      (struct Tree* tree, LOG_PARAMS);

int _tree_draw_legend   (FILE* graph, LOG_PARAMS);

int _graph_call_dot     (LOG_PARAMS);

int _print_node_data    (struct Node* node, FILE* output, LOG_PARAMS);

int _print_std_func     (int std_func_code, FILE* output, LOG_PARAMS);

int _print_node_data_type(struct Node* node, FILE* output, LOG_PARAMS);

//-------------------------------------------------------------------

int _node_init_identificator (struct Node* node, int64_t id_hash, LOG_PARAMS);

int _node_init_constant      (struct Node* node, double value, LOG_PARAMS);

int _node_init_key_node      (struct Node* node, int key_node_code, LOG_PARAMS);

int _node_init_bin_operand   (struct Node* node, int oper, LOG_PARAMS);

int _node_init_unar_operand  (struct Node* node, int oper, LOG_PARAMS);

int _node_init_std_func_node (struct Node* node, int code, LOG_PARAMS);

int _print_operand           (int oper_code, FILE* output, LOG_PARAMS);

int _print_key_node_data     (int key_node_code, FILE* output, LOG_PARAMS);

int _nodes_connect_right     (Node* parent, Node* son, LOG_PARAMS);

int _nodes_connect_left      (Node* parent, Node* son, LOG_PARAMS);

// int symb_is_var_name(char symb);

// int symb_is_operand(int symb);

// int is_function_operand(struct Node* node);

//===================================================================

#define nodes_connect_right(parent, son) \
       _nodes_connect_right(parent, son, LOG_ARGS)

#define nodes_connect_left(parent, son) \
       _nodes_connect_left(parent, son, LOG_ARGS)

#define print_operand(oper_code, output) \
       _print_operand(oper_code, output, LOG_ARGS)

#define print_key_node_data(key_node_code, output) \
       _print_key_node_data(key_node_code, output, LOG_ARGS)

#define print_node_data_type(node, output) \
       _print_node_data_type(node, output, LOG_ARGS)

#define node_save_to_file(node, output) \
       _node_save_to_file(node, output, LOG_ARGS)

#define tree_save_to_file(tree, filename) \
       _tree_save_to_file(tree, filename, LOG_ARGS)

#define node_dot_fill(node_dot, grapg_output, father) \
       _node_dot_fill(node_dot, grapg_output, father, LOG_ARGS)

#define graph_call_dot() \
       _graph_call_dot(LOG_ARGS)

#define tree_draw_legend(graph) \
       _tree_draw_legend(graph, LOG_ARGS)

#define node_draw_graph(node, node_dot) \
       _node_draw_graph(node, node_dot, LOG_ARGS)

#define tree_draw_graph(tree) \
       _tree_draw_graph(tree, LOG_ARGS)

#define tree_poisoning(tree) \
       _tree_poisoning(tree, LOG_ARGS)

#define tree_cleaning(tree) \
       _tree_cleaning(tree, LOG_ARGS)

#define tree_print(tree) \
       _tree_print(tree, LOG_ARGS)

#define tree_dump(tree) \
       _tree_dump(tree, LOG_ARGS)

#define node_print(tree) \
       _node_print(tree, LOG_ARGS)

#define tree_free_memory(tree) \
       _tree_free_memory(tree, LOG_ARGS)

#define node_allocate_memory() \
       _node_allocate_memory(LOG_ARGS)

#define tree_clear_check(tree) \
       _tree_clear_check(tree, LOG_ARGS)

#define tree_poison_check(tree) \
       _tree_poison_check(tree, LOG_ARGS)

#define tree_ctor(tree) \
       _tree_ctor(tree, LOG_ARGS)

#define tree_dtor(tree) \
       _tree_dtor(tree, LOG_ARGS) 

#define tree_validator(tree) \
       _tree_validator(tree, LOG_ARGS)

#define node_validator(node) \
       _node_validator(node, LOG_ARGS)

#define node_visiter(node, func) \
       _node_visiter(node, func, LOG_ARGS)

#define node_destruct(node) \
       _node_destruct(node, LOG_ARGS);                              

#define node_add_right_son(node) \
       _node_add_right_son(node, LOG_ARGS)

#define node_add_left_son(node) \
       _node_add_left_son(node, LOG_ARGS)

#define node_add_sons(node) \
       _node_add_sons(node, LOG_ARGS)

//#define node_init(node, value) _node_init(node, value, LOG_ARGS)

#define node_init_constant(node, value) \
       _node_init_constant(node, value, LOG_ARGS)

#define node_init_key_node(node, key_node_code) \
       _node_init_key_node(node, key_node_code, LOG_ARGS)

#define node_init_std_func_node(node, std_func_code) \
       _node_init_std_func_node(node, std_func_code, LOG_ARGS)

#define node_init_identificator(node, id_hash) \
       _node_init_identificator(node, id_hash, LOG_ARGS)

#define node_init_bin_operand(node, oper) \
       _node_init_bin_operand(node, oper, LOG_ARGS)

#define node_init_unar_operand(node, oper) \
       _node_init_unar_operand(node, oper, LOG_ARGS)

#define tree_delete_branch(node_ptr) \
       _tree_delete_branch(node_ptr, LOG_ARGS)

#define node_delete_branch(node) \
       _node_delete_branch(node, LOG_ARGS);           

#define print_node_data(node, output) \
       _print_node_data(node, output, LOG_ARGS) 

#define print_std_func(code, output) \
       _print_std_func(code, output, LOG_ARGS)     

#define get_node_dot_parameters(node, node_color, node_shape) \
       _get_node_dot_parameters(node, node_color, node_shape, LOG_ARGS)

//===================================================================