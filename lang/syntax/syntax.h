#pragma once

#include "../lang_common/lang_config.h"
#include "../lang_common/lang_common.h"
#include "../logs/errors_and_logs.h"
#include "../text_processing/text_processing.h"
#include "../tree/tree.h"
#include "names.h"

//===================================================================

#define SYNT_ERROR(error, tokens, names) {                          \
                                                                    \
    do                                                              \
    {                                                               \
        error_report(error);                                        \
        tokens_dump(tokens, logs_file);                             \
        names_struct_dump(names);                                   \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define TOKEN_IS_ID(token)        (token->type == ID)

#define TOKEN_IS_STD_FUNC(token)  (token->type == STD_FUNC)

#define TOKEN_IS_CMP_SIGN(token)  (token->type == CMP_SIGN)

#define TOKEN_IS_CALC_FUNC(token) (token->type == CALC_FUNC)

#define TOKEN_IS_CONSTANT(token)  (token->type == CONST)

#define TOKEN_IS_ADD(token)       (token->type == OPER && token->data.operand == ADD)

#define TOKEN_IS_SUB(token)       (token->type == OPER && token->data.operand == SUB)

#define TOKEN_IS_MUL(token)       (token->type == OPER && token->data.operand == MUL)

#define TOKEN_IS_DIV(token)       (token->type == OPER && token->data.operand == DIV)

#define TOKEN_IS_POW(token)       (token->type == OPER && token->data.operand == POW)

#define TOKEN_IS_DEN(token)       (token->type == OPER && token->data.operand == DEN)

#define TOKEN_IS_DEFN(token)      (token->type == KEY_WORD && token->data.key_word_code == DEFN)

#define TOKEN_IS_RET(token)       (token->type == KEY_WORD && token->data.key_word_code == RET)

#define TOKEN_IS_LABEL(token)     (token->type == KEY_WORD && token->data.key_word_code == LABEL)

#define TOKEN_IS_END(token)       (token->type == KEY_WORD && token->data.key_word_code == END)

#define TOKEN_IS_SOL(token)       (token->type == KEY_WORD && token->data.key_word_code == SOL)

#define TOKEN_IS_IF(token)        (token->type == KEY_WORD && token->data.key_word_code == IF)

#define TOKEN_IS_ELSE(token)      (token->type == KEY_WORD && token->data.key_word_code == ELSE)

#define TOKEN_IS_DECL(token)      (token->type == KEY_WORD && token->data.key_word_code == DECL)

#define TOKEN_IS_WHILE(token)     (token->type == KEY_WORD && token->data.key_word_code == WHILE)

#define TOKEN_IS_DO(token)        (token->type == KEY_WORD && token->data.key_word_code == DO)

#define TOKEN_IS_ASS(token)       (token->type == KEY_WORD && token->data.key_word_code == ASS)

#define TOKEN_IS_GOTO(token)      (token->type == KEY_WORD && token->data.key_word_code == GOTO)

#define TOKEN_IS_FBR_OPEN(token)  (token->type == KEY_WORD && token->data.key_word_code == FBR_OPEN)

#define TOKEN_IS_FBR_CLOSE(token) (token->type == KEY_WORD && token->data.key_word_code == FBR_CLOSE)

#define TOKEN_IS_PERM(token)      (token->type == KEY_WORD && token->data.key_word_code == PERM)

#define TOKEN_IS_BR_OPEN(token)   (token->type == KEY_WORD && token->data.key_word_code == BR_OPEN) 

#define TOKEN_IS_NEG(token)       (token->type == KEY_WORD && token->data.key_word_code == NEG)

#define TOKEN_IS_ARR(token)       (token->type == KEY_WORD && token->data.key_word_code == ARR)

#define TOKEN_IS_SBR_OPEN(token)  (token->type == KEY_WORD && token->data.key_word_code == SBR_OPEN)

#define TOKEN_IS_COMMA(token)     (token->type == KEY_WORD && token->data.key_word_code == COMMA)

#define TOKEN_IS_COPY(token)      (token->type == STD_FUNC && token->data.std_func_code == COPY)

#define TOKEN_IS_DRAW(token)      (token->type == STD_FUNC && token->data.std_func_code == DRAW)

//===================================================================

#define REQUIRE_KEY_WORD(key_code) {                                \
                                                                    \
    do                                                              \
    {                                                               \
        if (CUR_TOKEN->type != KEY_WORD                             \
        || CUR_TOKEN->data.key_word_code != key_code)               \
        {                                                           \
            error_report(REQUIRE_KEY_WORD);                         \
            tokens_dump(tokens, logs_file);                         \
            return NULL;                                            \
        }                                                           \
        else                                                        \
            tokens->counter++;                                      \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define REQUIRE_KEY_WORD_(key_code) {                               \
                                                                    \
    do                                                              \
    {                                                               \
        if (CUR_TOKEN->type != KEY_WORD                             \
        || CUR_TOKEN->data.key_word_code != key_code)               \
        {                                                           \
            error_report(REQUIRE_KEY_WORD);                         \
            tokens_dump(tokens, logs_file);                         \
            return -1;                                              \
        }                                                           \
        else                                                        \
            tokens->counter++;                                      \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define SYNTAX_READ_FUNC_START(tokens, names) {                     \
                                                                    \
    lang_log_report();                                              \
    TOKENS_PTR_CHECK(tokens);                                       \
    NAMES_PTR_CHECK(names);                                         \
}

//-------------------------------------------------------------------

#define SYNTAX_START_CHECK(tokens, names) {                         \
                                                                    \
    lang_log_report();                                              \
    TOKENS_STRUCT_PTR_CHECK(tokens);                                \
    NAMES_STRUCT_PTR_CHECK(names);                                  \
}

//===================================================================

#define TOKENS_PTR_CHECK(tokens) {                                  \
                                                                    \
    do                                                              \
    {                                                               \
        if (!tokens)                                                \
        {                                                           \
            error_report(INV_TOKENS_STRUCT_PTR);                    \
            return NULL;                                            \
        }                                                           \
    } while(0);                                                     \
}

//===================================================================

#define NULL_CHECK(ret) {                                           \
                                                                    \
    do                                                              \
    {                                                               \
        if (!ret)                                                   \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define RET_VALUE_CHECK(ret) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        if (ret == -1)                                              \
            return NULL;                                            \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

// int node_afterbuild_check  (Node* node FOR_LOGS(, LOG_PARAMS));

Node* _build_func_defn_constr(FOR_LOGS(LOG_PARAMS));

Node* _make_empty_array    (int size FOR_LOGS(, LOG_PARAMS));

Node* _build_size_constr   (int size FOR_LOGS(, LOG_PARAMS));

Node* _build_index_constr  (int index FOR_LOGS(, LOG_PARAMS));

int64_t _get_arr_name      (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

int _get_arr_size          (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

int _build_a_tree          (Tree* tree,   Tokens* tokens FOR_LOGS(, LOG_PARAMS));

Node* _get_g               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_definitions     (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_entity          (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_defn            (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_label_decl      (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_statement       (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_compl_statement (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_instruction     (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_cond            (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_ass             (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_sin_ass         (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_arr_ass         (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_decl            (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_arr_decl        (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_arr_el_index    (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_sin_decl        (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_cycle           (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_func_call       (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_std_func_call   (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_draw_func_call  (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_copy_func_call  (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_label_jump      (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_ret             (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_exp             (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_f               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_e               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_t               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_m               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_p               (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_id              (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_calc_func       (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_var_id          (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_var_id_decl     (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_func_id_decl    (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_label_id_decl   (Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _build_arr_ass_constr(int64_t hash, Node* array_values FOR_LOGS(, LOG_PARAMS));

Node* _get_array_values    (Tokens* tokens, Names* names, int size FOR_LOGS(, LOG_PARAMS));

Node* _build_arr_name      (int64_t name_hash, int size, int is_perm FOR_LOGS(, LOG_PARAMS));

Node* _get_func_call_args  (Tokens* tokens, Names* names, int arg_num FOR_LOGS(, LOG_PARAMS));

Node* _get_func_parameters (int* arg_num, Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

Node* _get_func_id         (Tokens* tokens, Names* names, int* name_hash FOR_LOGS(, LOG_PARAMS));


//-------------------------------------------------------------------

// #define node_afterbuild_check(node) 
//        _node_afterbuild_check(node FOR_LOGS(, LOG_ARGS))

#define build_arr_ass_constr(hash, node) \
       _build_arr_ass_constr(hash, node FOR_LOGS(, LOG_ARGS))

#define build_size_constr(size) \
       _build_size_constr(size FOR_LOGS(, LOG_ARGS))

#define build_index_constr(index) \
       _build_index_constr(index FOR_LOGS(, LOG_ARGS))

#define get_arr_el_index(tokens, names) \
       _get_arr_el_index(tokens, names FOR_LOGS(, LOG_ARGS))

#define build_arr_name(name_hash, size, is_perm) \
       _build_arr_name(name_hash, size, is_perm FOR_LOGS(, LOG_ARGS))

#define make_empty_array(size) \
       _make_empty_array(size FOR_LOGS(, LOG_ARGS))

#define build_func_defn_constr() \
       _build_func_defn_constr(FOR_LOGS(LOG_ARGS))

#define get_ret(tokens, names) \
       _get_ret(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_id(tokens, names) \
       _get_id(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_var_id(tokens, names) \
       _get_var_id(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_var_id_decl(tokens, names) \
       _get_var_id_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_func_id_decl(tokens, names) \
       _get_func_id_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_label_id_decl(tokens, names) \
       _get_label_id_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_func_parameters(arg_num, tokens, names) \
       _get_func_parameters(arg_num, tokens, names FOR_LOGS(, LOG_ARGS))

#define build_a_tree(tree, tokens) \
       _build_a_tree(tree, tokens FOR_LOGS(, LOG_ARGS))

#define get_g(tokens, names) \
       _get_g(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_definitions(tokens, names) \
       _get_definitions(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_entity(tokens, names) \
       _get_entity(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_defn(tokens, names) \
       _get_defn(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_label_decl(tokens, names) \
       _get_label_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_label_jump(tokens, names) \
       _get_label_jump(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_statement(tokens, names) \
       _get_statement(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_calc_func(tokens, names) \
       _get_calc_func(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_compl_statement(tokens, names) \
       _get_compl_statement(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_instruction(tokens, names) \
       _get_instruction(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_cond(tokens, names) \
       _get_cond(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_ass(tokens, names) \
       _get_ass(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_sin_ass(tokens, names) \
       _get_sin_ass(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_arr_ass(tokens, names) \
       _get_arr_ass(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_decl(tokens, names) \
       _get_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_sin_decl(tokens, names) \
       _get_sin_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_arr_decl(tokens, names) \
       _get_arr_decl(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_array_values(tokens, names, size) \
       _get_array_values(tokens, names, size FOR_LOGS(, LOG_ARGS))

#define get_arr_size(tokens, names) \
       _get_arr_size(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_arr_name(tokens, names) \
       _get_arr_name(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_cycle(tokens, names) \
       _get_cycle(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_func_call(tokens, names) \
       _get_func_call(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_func_call_args(tokens, names, arg_num) \
       _get_func_call_args(tokens, names, arg_num FOR_LOGS(, LOG_ARGS))

#define get_std_func_call(tokens, names) \
       _get_std_func_call(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_copy_func_call(tokens, names) \
       _get_copy_func_call(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_draw_func_call(tokens, names) \
       _get_draw_func_call(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_exp(tokens, names) \
       _get_exp(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_e(tokens, names) \
       _get_e(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_t(tokens, names) \
       _get_t(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_f(tokens, names) \
       _get_f(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_m(tokens, names) \
       _get_m(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_p(tokens, names) \
       _get_p(tokens, names FOR_LOGS(, LOG_ARGS))

#define get_func_id(tokens, names, name_hash) \
       _get_func_id(tokens, names, name_hash FOR_LOGS(, LOG_ARGS))

#define get_var_id(tokens, names) \
       _get_var_id(tokens, names FOR_LOGS(, LOG_ARGS))


