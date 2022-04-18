#pragma once

#include "../tree/tree.h"
#include "../lang_common/lang_common.h"
#include "../syntax/syntax.h"
#include "trans_config.h"

//=========================================================

const int Start_var_arr_size = 15;

//=========================================================

struct Var
{
    int64_t hash;
    int ram_pos;
    int size;
};

//---------------------------------------------------------

struct Nspace
{
    int var_num;
    int var_cap;
    Var* vars;

    int local_ram_pos;
};

//---------------------------------------------------------

struct Trans
{
    Tree* tree;
    FILE* asm_file;

    Nspace* nspaces;
    Nspace* cur_nspace;
    int nspaces_num;

    int ram_pos;
};

//=========================================================

#define TRANS_START_CHECK(node, trans) {                    \
                                                            \
    do                                                      \
    {                                                       \
        lang_log_report();                                  \
        NODE_PTR_CHECK(node);                               \
        TRANS_STRUCT_PTR_CHECK(trans);                      \
        FILE_PTR_CHECK(trans->asm_file);                    \
        TREE_PTR_CHECK(trans->tree);                        \
                                                            \
    } while(0);                                             \
}

//---------------------------------------------------------

#define TRANS_PTR_CHECK(trans) {                            \
                                                            \
    do                                                      \
    {                                                       \
        if (!trans)                                         \
        {                                                   \
            error_report(INV_TRANS_PTR);                    \
            return NULL;                                    \
        }                                                   \
    } while(0);                                             \
}

#define TRANS_STRUCT_PTR_CHECK(trans) {                     \
                                                            \
    do                                                      \
    {                                                       \
        if (!trans)                                         \
        {                                                   \
            error_report(INV_TRANS_PTR);                    \
            return -1;                                      \
        }                                                   \
    } while(0);                                             \
}

//=========================================================

#define NODE_IS_STAT(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == STATEMENT_ND)

#define NODE_IS_GOTO(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == GOTO_ND     )

#define NODE_IS_PERM(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == PERM_ND     )

#define NODE_IS_LABEL(node    )  (node->data_type == KEY_NODE && node->data.key_node_code == LABEL_ND    )

#define NODE_IS_IF(node       )  (node->data_type == KEY_NODE && node->data.key_node_code == IF_ND       )

#define NODE_IS_ASS(node      )  (node->data_type == KEY_NODE && node->data.key_node_code == ASS_ND      )

#define NODE_IS_DECL(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == DECL_ND     )

#define NODE_IS_WHILE(node    )  (node->data_type == KEY_NODE && node->data.key_node_code == WHILE_ND    )

#define NODE_IS_CALL(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == CALL_ND     )

#define NODE_IS_RET(node      )  (node->data_type == KEY_NODE && node->data.key_node_code == RET_ND      )

#define NODE_IS_ELEM(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == ELEM_ND     )

#define NODE_IS_INDEX(node    )  (node->data_type == KEY_NODE && node->data.key_node_code == INDEX_ND    )

#define NODE_IS_SIZE(node     )  (node->data_type == KEY_NODE && node->data.key_node_code == SIZE_ND     )

#define NODE_IS_PARAM(node    )  (node->data_type == KEY_NODE && node->data.key_node_code == PARAMETER_ND)

#define NODE_IS_CONSTANT(node )  (node->data_type == CONSTANT     )

#define NODE_IS_ID(node       )  (node->data_type == IDENTIFICATOR)

#define NODE_IS_UNAR_OPER(node)  (node->data_type == UNAR_OPERAND )

#define NODE_IS_BIN_OPER(node )  (node->data_type == BIN_OPERAND  )

#define NODE_IS_STD_FUNC(node )  (node->data_type == STD_FUNC_NODE)

#define NODE_IS_KEY_NODE(node )  (node->data_type == KEY_NODE     )

//=========================================================

int _node_is_cmp_sign   (Node* node FOR_LOGS(, LOG_PARAMS));

int _write_asm_preparations(FILE* asm_file FOR_LOGS(, LOG_PARAMS));

int _trans_func_args(Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_compl_stat     (Node* node, Trans* trans FOR_LOGS(,LOG_PARAMS));

int _trans_entities       (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_statement      (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_label_decl     (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_cond           (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_ass            (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_ass_index      (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_decl           (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_cycle          (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_func_call      (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_func_call_args (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_std_func_call  (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_label_jump     (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_ret            (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_exp            (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_unary_oper     (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_binary_oper    (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_print          (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_scan           (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_draw           (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_copy           (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_constant       (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_variable       (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_bin_operand    (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_unar_operand   (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_definitions    (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_func_defn      (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_var_pop        (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_var_push       (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_pop_offset     (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_parameter      (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_func_parameters(Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_tree_to_asm    (Tree* tree, FILE* asm_file FOR_LOGS(, LOG_PARAMS));

int _translation_execute(Tree* tree, const char* asm_filename FOR_LOGS(, LOG_PARAMS));

//===================================================================

int _trans_struct_ctor(Trans* trans, FILE* asm_file, Tree* tree 
                                       FOR_LOGS(, LOG_PARAMS));

int _add_nspace       (Trans* trans FOR_LOGS(, LOG_PARAMS));

int _rm_nspace        (Trans* trans FOR_LOGS(, LOG_PARAMS));

int _var_arr_increase (Nspace* nspace FOR_LOGS(, LOG_PARAMS));

int _add_var_decl     (Trans* trans, int64_t var_hash, int size FOR_LOGS(, LOG_PARAMS));

int _was_var_decl     (Trans* trans, int64_t hash FOR_LOGS(, LOG_PARAMS));

int _get_var_ram_pos  (Trans* trans, int64_t hash FOR_LOGS(, LOG_PARAMS));

int _get_size_of_var  (Trans* trans, int64_t hash FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dump(Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dtor(Trans* trans FOR_LOGS(, LOG_PARAMS));

int _move_memory_place(Trans* trans FOR_LOGS(, LOG_PARAMS));

int _move_memory_place_back(int offset, FILE* asm_file FOR_LOGS(, LOG_PARAMS));

int _reset_ram_pos    (Trans* trans FOR_LOGS(, LOG_PARAMS));

int _trans_var        (Trans* trans, int64_t hash FOR_LOGS(, LOG_PARAMS));

int _trans_arr_values (Node* node, Trans* trans FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define reset_ram_pos(trans) \
       _reset_ram_pos(trans FOR_LOGS(, LOG_ARGS))

#define trans_struct_ctor(trans, asm_file, tree) \
       _trans_struct_ctor(trans, asm_file, tree FOR_LOGS(, LOG_ARGS))

#define trans_struct_dtor(trans) \
       _trans_struct_dtor(trans FOR_LOGS(, LOG_ARGS))
       
#define trans_struct_dump(trans) \
       _trans_struct_dump(trans FOR_LOGS(, LOG_ARGS))

#define add_nspace(trans) \
       _add_nspace(trans FOR_LOGS(, LOG_ARGS))

#define rm_nspace(trans) \
       _rm_nspace(trans FOR_LOGS(, LOG_ARGS))

#define var_arr_increase(nspace) \
       _var_arr_increase(nspace FOR_LOGS(, LOG_ARGS))

#define add_var_decl(trans, var_hash, size) \
       _add_var_decl(trans, var_hash, size FOR_LOGS(, LOG_ARGS))

#define was_var_decl(trans, var_hash) \
       _was_var_decl(trans, var_hash FOR_LOGS(, LOG_ARGS))

#define get_var_ram_pos(trans, hash) \
       _get_var_ram_pos(trans, hash FOR_LOGS(, LOG_ARGS))

#define get_size_of_var(trans, hash) \
       _get_size_of_var(trans, hash FOR_LOGS(, LOG_ARGS))

#define move_memory_place(trans) \
       _move_memory_place(trans FOR_LOGS(, LOG_ARGS))

#define move_memory_place_back(offset, asm_file) \
       _move_memory_place_back(offset, asm_file FOR_LOGS(, LOG_ARGS))

//===================================================================

#define write_asm_preparations(asm_file) \
       _write_asm_preparations(asm_file FOR_LOGS(, LOG_ARGS))

#define trans_definitions(node, trans) \
       _trans_definitions(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_var_push(node, trans) \
       _trans_var_push(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_var_pop(node, trans) \
       _trans_var_pop(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_pop_offset(node, trans) \
       _trans_pop_offset(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_func_defn(node, trans) \
       _trans_func_defn(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_constant(node, trans) \
       _trans_constant(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_entities(node, trans) \
       _trans_entities(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_compl_stat(node, trans) \
       _trans_compl_stat(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_variable(node, trans) \
       _trans_variable(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_unar_operand(node, trans) \
       _trans_unar_operand(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_bin_operand(node, trans) \
       _trans_bin_operand(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_print(node, trans) \
       _trans_print(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_draw(node, trans) \
       _trans_draw(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_copy(node, trans) \
       _trans_copy(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_parameter(node, trans) \
       _trans_parameter(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_func_parameters(node, trans) \
       _trans_func_parameters(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_scan(node, trans) \
       _trans_scan(node, trans FOR_LOGS(, LOG_ARGS))

#define translation_execute(tree, filename) \
       _translation_execute(tree, filename FOR_LOGS(, LOG_ARGS))

#define node_is_cmp_sign(node) \
       _node_is_cmp_sign(node FOR_LOGS(, LOG_ARGS))

#define trans_tree_to_asm(tree, trans) \
       _trans_tree_to_asm(tree, trans FOR_LOGS(, LOG_ARGS))

#define trans_statement(node, trans) \
       _trans_statement(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_label_decl(node, trans) \
       _trans_label_decl(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_cond(node, trans) \
       _trans_cond(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_ass(node, trans) \
       _trans_ass(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_ass_index(node, trans) \
       _trans_ass_index(node, trans FOR_LOGS(, LOG_ARGS))

// #define trans_arr_ass(node, trans) 
       // _trans_arr_ass(node, trans FOR_LOGS(, LOG_ARGS))

// #define trans_arr_el_ass(node, trans) 
//        _trans_arr_el_ass(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_decl(node, trans) \
       _trans_decl(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_cycle(node, trans) \
       _trans_cycle(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_std_func_call(node, trans) \
       _trans_std_func_call(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_func_call(node, trans) \
       _trans_func_call(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_func_call_args(node, tras) \
       _trans_func_call_args(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_label_jump(node, trans) \
       _trans_label_jump(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_ret(node, trans) \
       _trans_ret(node, trans FOR_LOGS(, LOG_ARGS))

#define trans_exp(node, trans) \
       _trans_exp(node, trans FOR_LOGS(, LOG_ARGS))






