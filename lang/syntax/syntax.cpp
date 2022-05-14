#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "syntax.h"
#include "../general/general.h"

//===================================================================

#define CUR_TOKEN tokens->token_array[tokens->counter]

#define N_TOKEN tokens->token_array[tokens->counter + 1]

#define N_N_TOKEN tokens->token_array[tokens->counter + 2]

#define PR_TOKEN tokens->token_array[tokens->counter - 1]

//--------------------------------------------------------------------

#define L left_son
#define R right_son

#define NL node->left_son

#define NR node->right_son

#define NLL node->left_son->left_son

#define NLR node->left_son->right_son

#define NRR node->right_son->right_son

#define NRL node->right_son->left_son

//===================================================================

// int node_afterbuild_check(Node* node FOR_LOGS(, LOG_PARAMS))
// {
//     NODE_PTR_CHECK(node);

//     return 0;
// }


//===================================================================

int _build_a_tree(Tree* tree, Tokens* tokens FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    TREE_PTR_CHECK(tree);
    TOKENS_STRUCT_PTR_CHECK(tokens);

    Names names = { 0 };
    int ret = init_names_struct(&names);
    RETURN_CHECK(ret);

    Node* root = get_g(tokens, &names);
    if (!root)
        return -1;

    tree->root = root;
    //tree_draw_graph(tree);

    ret = kill_names_struct(&names);
    RETURN_CHECK(ret);

    return 0;
}

//-------------------------------------------------------------------

Node* _get_g(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);

    NODE_INIT_KEY_NODE(node, HEAD_ND);
    node->parent = No_parent;

    int check = 0;

    if (TOKEN_IS_DEFN(CUR_TOKEN))
    {
        node->right_son = get_definitions(tokens, names);
        CONNECT_RIGHT(node, node->right_son);

        check = add_var_space(names->var_cluster);
        RET_VALUE_CHECK(check);
    }

    REQUIRE_KEY_WORD(START);

    Node* entity = get_entity(tokens, names);
    NULL_CHECK(entity);

    CONNECT_LEFT(node, entity);

    while (tokens->counter < tokens->amount - 1)
    {
        Node* new_entity = get_entity(tokens, names);
        NULL_CHECK(new_entity);

        CONNECT_LEFT(entity, new_entity);
        entity = new_entity;
    }

    //
    //names_struct_dump(names);
    //

    check = clear_var_spaces(names->var_cluster);
    RET_VALUE_CHECK(check);
    
    REQUIRE_KEY_WORD(END);

    if (tokens->counter != tokens->amount)
    {
        SYNT_ERROR(TOKENS_AFTER_END, tokens, names);
        return NULL;
    }

    return node;
}

//-------------------------------------------------------------------

Node* _get_definitions(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* defn = get_defn(tokens, names);
    NULL_CHECK(defn);

        //
        //names_struct_dump(names);
        //

    int check = clear_var_spaces(names->var_cluster);
    RET_VALUE_CHECK(check);

        //
        //names_struct_dump(names);
        //

    Node* first_defn = defn;

    while (TOKEN_IS_DEFN(CUR_TOKEN))
    {
        check = add_var_space(names->var_cluster);
        RET_VALUE_CHECK(check);

        Node* next_defn = get_defn(tokens, names);
        NULL_CHECK(next_defn);

        CONNECT_LEFT(defn, next_defn);
        defn = next_defn;

        check = clear_var_spaces(names->var_cluster);
        RET_VALUE_CHECK(check);
    }

    return first_defn;
}

//-------------------------------------------------------------------

Node* _get_entity(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = NULL;

    if (TOKEN_IS_LABEL(N_TOKEN))
    {
        node = get_label_decl(tokens, names);
        NULL_CHECK(node);
    }

    else
    {
        node = get_statement(tokens, names);
        NULL_CHECK(node);
    }

    return node;
}

//-------------------------------------------------------------------

Node* _build_func_defn_constr(FOR_LOGS(LOG_PARAMS))
{
    lang_log_report();
    
    Node* func_defn_node = (Node*)node_allocate_memory();
    NULL_CHECK(func_defn_node);
    NODE_INIT_KEY_NODE(func_defn_node, FUNC_DEFN_ND);

    ADD_RIGHT(func_defn_node);
    NODE_INIT_KEY_NODE(func_defn_node->R, DEFN_ND);

    return func_defn_node;
;}

//-------------------------------------------------------------------

Node* _get_defn(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(DEFN);

    Node* func_defn_nd = build_func_defn_constr();
    NULL_CHECK(func_defn_nd);

    Node* func_nd = get_func_id_decl(tokens, names);
    NULL_CHECK(func_nd);
    CONNECT_LEFT(func_defn_nd->R, func_nd);

    // Node* param_node = get_func_parameters(tokens, names);
    // NULL_CHECK(param_node);
    // CONNECT_RIGHT(func_defn_nd->R->L, param_node);

    Node* statement = get_compl_statement(tokens, names);
    NULL_CHECK(statement);
    CONNECT_RIGHT(func_defn_nd->R, statement);

    return func_defn_nd;
}

//-------------------------------------------------------------------

Node* _get_func_parameters(int* arg_num, Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(BR_OPEN);

    Node* param_nd = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(param_nd, PARAMETER_ND);

    CONNECT_RIGHT(param_nd, get_var_id_decl(tokens, names));

    Node* returning = param_nd;

    int arg_counter = 1;

    while (TOKEN_IS_COMMA(CUR_TOKEN))
    {
        tokens->counter++;
        arg_counter++;

        ADD_LEFT(param_nd);
        NODE_INIT_KEY_NODE(param_nd->L, PARAMETER_ND);

        CONNECT_RIGHT(param_nd->L, get_var_id_decl(tokens, names))
        param_nd = param_nd->L;
    }

    REQUIRE_KEY_WORD(BR_CLOSE);
    *arg_num = arg_counter;

    return returning;
}

//-------------------------------------------------------------------

Node* _get_label_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);
    REQUIRE_KEY_WORD(LABEL);

    Node* label_name = get_label_id_decl(tokens, names);
    NULL_CHECK(label_name);

    Node* label_nd = (Node*)node_allocate_memory();
    NULL_CHECK(label_nd);

    CONNECT_RIGHT(label_nd, label_name);

    REQUIRE_KEY_WORD(EOL);

    return label_nd;
}

//-------------------------------------------------------------------

Node* _get_statement(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(node, STATEMENT_ND);

    if (TOKEN_IS_IF(CUR_TOKEN))
    {
        Node* son = get_cond(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_STD_FUNC(CUR_TOKEN))
    {
        Node* son = get_std_func_call(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_DECL(CUR_TOKEN))
    {
        Node* son = get_decl(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_WHILE(CUR_TOKEN))
    {
        Node* son = get_cycle(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_RET(CUR_TOKEN))
    {
        Node* son = get_ret(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_GOTO(CUR_TOKEN))
    {
        Node* son = get_label_jump(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else if (TOKEN_IS_ID(CUR_TOKEN) && TOKEN_IS_BR_OPEN(N_TOKEN))
    {
        Node* son = get_func_call(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    else /*if (TOKEN_IS_ASS(N_TOKEN)) */
    {
        Node* son = get_ass(tokens, names);
        NULL_CHECK(son);
        CONNECT_RIGHT(node, son);
    }

    // else
    // {
    //     SYNT_ERROR(MISSING_STATEMENT, tokens, names);
    //     return NULL;
    // }

    REQUIRE_KEY_WORD(EOL);
    return node;
}

//-------------------------------------------------------------------

Node* _get_compl_statement(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(FBR_OPEN);

    Node* statement = get_statement(tokens, names);
    NULL_CHECK(statement);

    Node* first_stat = statement;

    while (!TOKEN_IS_FBR_CLOSE(CUR_TOKEN))
    {
        Node* new_statement = get_statement(tokens, names);
        NULL_CHECK(new_statement);

        CONNECT_LEFT(statement, new_statement);
        statement = new_statement;
    }

    REQUIRE_KEY_WORD(FBR_CLOSE);

    return first_stat;
}

//--------------------------FF-----------------------------------------

Node* _get_ret(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(RET);

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);
    NODE_INIT_KEY_NODE(node, RET_ND);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);

    CONNECT_RIGHT(node, exp);

    return node;
}
//-------------------------------------------------------------------

Node* _get_instruction(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    //
    //names_struct_dump(names);
    //

    int ret = add_var_space(names->var_cluster);
    RET_VALUE_CHECK(ret);

    //
    //names_struct_dump(names);
    //

    Node* node = 0;

    if (TOKEN_IS_FBR_OPEN(CUR_TOKEN))
    {
        node = get_compl_statement(tokens, names);
        NULL_CHECK(node);
    }

    else
    {
        node = get_statement(tokens, names);
        NULL_CHECK(node);
    }

    //
    //names_struct_dump(names);
    //

    ret = rm_var_space(names->var_cluster);
    RET_VALUE_CHECK(ret);

    //
    //names_struct_dump(names);
    //

    return node;
}

//-------------------------------------------------------------------

Node* _get_cond(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(IF);
    REQUIRE_KEY_WORD(BR_OPEN);

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);

    NODE_INIT_KEY_NODE(node, IF_ND);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);
    CONNECT_RIGHT(node, exp);

    REQUIRE_KEY_WORD(BR_CLOSE);
    REQUIRE_KEY_WORD(DO);

    ADD_LEFT(node);
    NODE_INIT_KEY_NODE(NL, DECISION_ND);

    Node* ttrue = get_instruction(tokens, names);
    NULL_CHECK(ttrue);
    CONNECT_RIGHT(NL, ttrue);

    if (TOKEN_IS_ELSE(CUR_TOKEN))
    {
        tokens->counter++;

        Node* ffalse = get_instruction(tokens, names);
        NULL_CHECK(ffalse);
        CONNECT_LEFT(NL, ffalse);
    }

    return node;
}

//-------------------------------------------------------------------

Node* _get_ass(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* ass = NULL;

    if (TOKEN_IS_ARR(CUR_TOKEN))
    {
        ass = get_arr_ass(tokens, names);
        NULL_CHECK(ass);
    }

    else
    {
        ass = get_sin_ass(tokens, names);
        NULL_CHECK(ass);
    }

    return ass;
}

//-------------------------------------------------------------------

Node* _get_arr_ass(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(ARR);

    int64_t var_hash = 0;

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(INV_LEFT_VALUE, tokens, names);
        return NULL;
    }
    else
    {
        if (!var_is_declared(CUR_TOKEN->data.id_hash, names->var_cluster))
        {
            SYNT_ERROR(VAR_UNDECLARED, tokens, names);
            return NULL;
        }
        else if (var_is_permanent(CUR_TOKEN->data.id_hash, names->var_cluster))
        {
            SYNT_ERROR(ASS_TO_PERM, tokens, names);
            return NULL;
        }
        
        var_hash = CUR_TOKEN->data.id_hash;
        tokens->counter++;
    }

    int var_size = get_var_size(var_hash, names->var_cluster);
    RET_VALUE_CHECK(var_size);

    if (var_size <= 1)
    {
        SYNT_ERROR(ARR_KEY_WORD_WITH_VAR, tokens, names);
        return NULL;
    }

    REQUIRE_KEY_WORD(SBR_OPEN);
    REQUIRE_KEY_WORD(SBR_CLOSE);

    REQUIRE_KEY_WORD(ASS);

    Node* array_values = get_array_values(tokens, names, var_size);
    NULL_CHECK(array_values);

    return build_arr_ass_constr(var_hash, array_values);
}

//-------------------------------------------------------------------

Node* _build_arr_ass_constr(int64_t hash, Node* array_values FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    if (!array_values)
    {
        error_report(INV_NODE_PTR);
        return NULL;
    }

    Node* ass_nd = (Node*)node_allocate_memory();
    NULL_CHECK(ass_nd);

    NODE_INIT_KEY_NODE(ass_nd, ASS_ND);

    ADD_LEFT(ass_nd);
    NODE_INIT_IDENTIFICATOR(ass_nd->L, hash);

    CONNECT_RIGHT(ass_nd, array_values);

    return ass_nd;
}

//-------------------------------------------------------------------

Node* _get_sin_ass(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(node, ASS_ND);

    int64_t var_hash = 0;

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(INV_LEFT_VALUE, tokens, names);
        return NULL;
    }
    else
    {
        if (!var_is_declared(CUR_TOKEN->data.id_hash, names->var_cluster))
        {
            SYNT_ERROR(VAR_UNDECLARED, tokens, names);
            return NULL;
        }
        else if (var_is_permanent(CUR_TOKEN->data.id_hash, names->var_cluster))
        {
            SYNT_ERROR(ASS_TO_PERM, tokens, names);
            return NULL;
        }

        ADD_LEFT(node);
        var_hash = CUR_TOKEN->data.id_hash;

        NODE_INIT_IDENTIFICATOR(NL, var_hash);
        tokens->counter++;
    }

    int var_size = get_var_size(var_hash, names->var_cluster);
    RET_VALUE_CHECK(var_size);

    if (TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size == 1)
    {
        SYNT_ERROR(VAR_WITH_INDEX, tokens, names);
        return NULL;
    }

    else if (!TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size > 1)
    {
        SYNT_ERROR(ARRAY_WITH_NO_INDEX, tokens, names);
        return NULL;
    }
    
    else if (TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size > 1)
    {
        Node* el_index = get_arr_el_index(tokens, names); //
        NULL_CHECK(el_index);

        CONNECT_RIGHT(NL, el_index);
    }

    else
        CONNECT_RIGHT(NL, build_index_constr(0));

    REQUIRE_KEY_WORD(ASS);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);
    CONNECT_RIGHT(node, exp);

    return node;
}

//------------------------------------------------------------------

Node* _build_size_constr(int size FOR_LOGS(, LOG_PARAMS))
{
    Node* size_nd = (Node*)node_allocate_memory();
    NULL_CHECK(size_nd);

    NODE_INIT_KEY_NODE(size_nd, SIZE_ND);

    ADD_RIGHT(size_nd);
    NODE_INIT_CONSTANT(size_nd->R, size);

    return size_nd;
}

//------------------------------------------------------------------

Node* _build_index_constr(int index FOR_LOGS(, LOG_PARAMS))
{
    Node* index_nd = (Node*)node_allocate_memory();
    NULL_CHECK(index_nd);

    NODE_INIT_KEY_NODE(index_nd, INDEX_ND);

    ADD_RIGHT(index_nd);
    NODE_INIT_CONSTANT(index_nd->R, index);

    return index_nd;
}

//------------------------------------------------------------------

Node* _get_arr_el_index(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(SBR_OPEN);

    Node* index_nd = (Node*)node_allocate_memory();
    NULL_CHECK(index_nd);

    NODE_INIT_KEY_NODE(index_nd, INDEX_ND);

    Node* el_index = get_exp(tokens, names);
    NULL_CHECK(el_index);

    CONNECT_RIGHT(index_nd, el_index);

    REQUIRE_KEY_WORD(SBR_CLOSE);

    return index_nd;
}

//-------------------------------------------------------------------

Node* _get_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(DECL);

    Node* decl = NULL;

    if (TOKEN_IS_ARR(CUR_TOKEN) 
    || (TOKEN_IS_ARR(N_TOKEN) && TOKEN_IS_PERM(CUR_TOKEN)))
    {
        decl = get_arr_decl(tokens, names);
        NULL_CHECK(decl);
    }

    else
    {
        decl = get_sin_decl(tokens, names);
        NULL_CHECK(decl);
    }

    return decl;
}

//-------------------------------------------------------------------

Node* _get_arr_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* decl = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(decl, DECL_ND);

    ADD_RIGHT(decl);
    Node* ass = decl->R;
    NODE_INIT_KEY_NODE(ass, ASS_ND);

    int is_perm = 0;

    if (TOKEN_IS_PERM(CUR_TOKEN))
    {
        tokens->counter++;
        is_perm = 1;
    }

    REQUIRE_KEY_WORD(ARR);

    int64_t name_hash = get_arr_name(tokens, names);
    RET_VALUE_CHECK(name_hash);

    int size = get_arr_size(tokens, names);
    RET_VALUE_CHECK(size);

    REQUIRE_KEY_WORD(ASS);

    int is_ok = add_var_declare(name_hash, is_perm, size, names->var_cluster);
    RET_VALUE_CHECK(is_ok);

    Node* array_values = get_array_values(tokens, names, size);
    NULL_CHECK(array_values);

    CONNECT_LEFT(ass, build_arr_name(name_hash, size, is_perm));
    CONNECT_RIGHT(ass, array_values);

    return decl;
}

//-------------------------------------------------------------------

Node* _build_arr_name(int64_t name_hash, int size , int is_perm FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();

    Node* name = (Node*)node_allocate_memory();
    NODE_INIT_IDENTIFICATOR(name, name_hash);

    ADD_RIGHT(name);
    NODE_INIT_KEY_NODE(name->R, SIZE_ND);

    ADD_RIGHT(name->R);
    NODE_INIT_CONSTANT(name->R->R, size);

    if (is_perm)
    {
        Node* perm_nd = (Node*)node_allocate_memory();
        NODE_INIT_KEY_NODE(perm_nd, PERM_ND);

        CONNECT_RIGHT(perm_nd, name);
        return perm_nd;
    }

    return name;
}

//-------------------------------------------------------------------

Node* _get_array_values(Tokens* tokens, Names* names, int size FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(FBR_OPEN);

    if (TOKEN_IS_FBR_CLOSE(CUR_TOKEN))
    {
        tokens->counter++;    
        return make_empty_array(size);
    }

    Node* elem_nd = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(elem_nd, ELEM_ND);

    CONNECT_RIGHT(elem_nd, get_exp(tokens, names));
    NULL_CHECK(elem_nd->R);

    Node* returning = elem_nd;

    int numbers_read = 1;

    for (int counter = 1; counter < size && TOKEN_IS_COMMA(CUR_TOKEN); counter++)
    {
        REQUIRE_KEY_WORD(COMMA);

        ADD_LEFT(elem_nd);
        NODE_INIT_KEY_NODE(elem_nd->L, ELEM_ND);

        CONNECT_RIGHT(elem_nd->L, get_exp(tokens, names));
        NULL_CHECK(elem_nd->L);

        elem_nd = elem_nd->L;
        numbers_read++;
    }

    if (numbers_read != size)
        CONNECT_LEFT(elem_nd, make_empty_array(size - numbers_read));
    

    REQUIRE_KEY_WORD(FBR_CLOSE);

    return returning;
}

//-------------------------------------------------------------------

Node* _make_empty_array(int size FOR_LOGS(, LOG_PARAMS))
{
    if (size <= 0)
    {
        error_report(INV_ARR_SIZE);
        return NULL;
    }

    Node* elem_nd = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(elem_nd, ELEM_ND);

    ADD_RIGHT(elem_nd);
    NODE_INIT_CONSTANT(elem_nd->R, 0);

    Node* returning = elem_nd;

    for (int counter = 1; counter < size; counter++)
    {
        ADD_LEFT(elem_nd);
        NODE_INIT_KEY_NODE(elem_nd->L, ELEM_ND);

        ADD_RIGHT(elem_nd->L);
        NODE_INIT_CONSTANT(elem_nd->L->R, 0);

        elem_nd = elem_nd->L;
    }

    return returning;
}

//-------------------------------------------------------------------

int _get_arr_size(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_START_CHECK(tokens, names);

    REQUIRE_KEY_WORD_(SBR_OPEN);

    if (!TOKEN_IS_CONSTANT(CUR_TOKEN))
    {
        SYNT_ERROR(INV_ARR_SIZE, tokens, names);
        return -1;
    }

    int size = (int)CUR_TOKEN->data.constant;
    tokens->counter++;

    REQUIRE_KEY_WORD_(SBR_CLOSE);

    return size;
}

//-------------------------------------------------------------------

int64_t _get_arr_name(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_START_CHECK(tokens, names);

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return -1;
    }

    else if (var_is_declared(CUR_TOKEN->data.id_hash, names->var_cluster))
    {
        SYNT_ERROR(VAR_REDECLARE, tokens, names);
        return -1;
    }

    int64_t name_hash = CUR_TOKEN->data.id_hash;
    tokens->counter++;

    return name_hash;
}

//-------------------------------------------------------------------

Node* _get_sin_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* var = get_var_id_decl(tokens, names);
    NULL_CHECK(var);

    REQUIRE_KEY_WORD(ASS);

    Node* decl = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(decl, DECL_ND);
    NULL_CHECK(decl);

    Node* ass = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(ass, ASS_ND);
    NULL_CHECK(ass);

    CONNECT_RIGHT(decl, ass);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);

    CONNECT_RIGHT(ass, exp);
    CONNECT_LEFT(ass, var);

    return decl;
}

//-------------------------------------------------------------------

Node* _get_cycle(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(WHILE);
    REQUIRE_KEY_WORD(BR_OPEN);

    Node* node = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(node, WHILE_ND);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);
    CONNECT_RIGHT(node, exp);

    REQUIRE_KEY_WORD(BR_CLOSE);
    REQUIRE_KEY_WORD(DO);

    Node* instr = get_instruction(tokens, names);
    NULL_CHECK(instr);
    CONNECT_LEFT(node, instr);

    return node;
}

//-------------------------------------------------------------------

Node* _get_std_func_call(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    if (TOKEN_IS_DRAW(CUR_TOKEN))
        return get_draw_func_call(tokens, names);

    if (TOKEN_IS_COPY(CUR_TOKEN))
        return get_copy_func_call(tokens, names);

    Node* node = (Node*)node_allocate_memory();
    int func_code = CUR_TOKEN->data.std_func_code;

    NODE_INIT_STD_FUNC_NODE(node, func_code);
    tokens->counter++;

    REQUIRE_KEY_WORD(BR_OPEN);

    if (func_code != SCAN)
    {
        Node* exp = get_exp(tokens, names);
        NULL_CHECK(exp);
        CONNECT_RIGHT(node, exp);

        REQUIRE_KEY_WORD(BR_CLOSE);
        return node;
    }

    Node* arg = get_var_id(tokens, names);
    NULL_CHECK(arg);
    CONNECT_RIGHT(node, arg);

    REQUIRE_KEY_WORD(BR_CLOSE);

    return node;
}

//-------------------------------------------------------------------

Node* _get_copy_func_call(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* copy = (Node*)node_allocate_memory();
    NULL_CHECK(copy);

    NODE_INIT_STD_FUNC_NODE(copy, COPY);
    tokens->counter++;

    REQUIRE_KEY_WORD(BR_OPEN);

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (!var_is_declared(CUR_TOKEN->data.id_hash, names->var_cluster))
    {
        SYNT_ERROR(VAR_UNDECLARED, tokens, names);
        return NULL;
    }

    else if (get_var_size(CUR_TOKEN->data.id_hash, names->var_cluster) 
                                        != FRAME_LENGTH * FRAME_WIDTH)
    {
        SYNT_ERROR(INV_COPY_ARG_SIZE, tokens, names);
        return NULL;
    }

    ADD_RIGHT(copy);
    NODE_INIT_IDENTIFICATOR(copy->R, CUR_TOKEN->data.id_hash);
    tokens->counter++;

    REQUIRE_KEY_WORD(BR_CLOSE);

    return copy;
}

//-------------------------------------------------------------------

Node* _get_draw_func_call(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* draw = (Node*)node_allocate_memory();
    NULL_CHECK(draw);

    NODE_INIT_STD_FUNC_NODE(draw, DRAW);
    tokens->counter++;

    REQUIRE_KEY_WORD(BR_OPEN);
    REQUIRE_KEY_WORD(BR_CLOSE);

    return draw;
}
//-------------------------------------------------------------------

Node* _get_func_call(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(node, CALL_ND);
    Node* call = node;

    ADD_LEFT(node);
    node = NL;

    NODE_INIT_KEY_NODE(node, FUNC_ND);

    int arg_num = 0;

    Node* func_name = get_func_id(tokens, names, &arg_num);
    NULL_CHECK(func_name);
    CONNECT_LEFT(node, func_name);

    REQUIRE_KEY_WORD(BR_OPEN);

    CONNECT_RIGHT(node, get_func_call_args(tokens, names, arg_num));
    NULL_CHECK(NR);

    REQUIRE_KEY_WORD(BR_CLOSE);

    return call;
}

//-------------------------------------------------------------------

Node* _get_func_call_args(Tokens* tokens, Names* names, int arg_num FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* param_nd = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(param_nd, PARAMETER_ND);

    CONNECT_RIGHT(param_nd, get_exp(tokens, names));
    NULL_CHECK(param_nd->R);

    Node* returning = param_nd;
    int arg_counter = 1;

    while (TOKEN_IS_COMMA(CUR_TOKEN))
    {
        tokens->counter++;

        ADD_LEFT(param_nd);
        NODE_INIT_KEY_NODE(param_nd->L, PARAMETER_ND);

        CONNECT_RIGHT(param_nd->L, get_exp(tokens, names));
        NULL_CHECK(param_nd->L->R);
    
        param_nd = param_nd->L;
        arg_counter++;
    }

    //fprintf(logs_file, "\n<pre>\n\n\n\n arg_num %d arg_ct %d \n\n\n </pre>\n", arg_num, arg_counter);

    if (arg_counter != arg_num)
    {
        SYNT_ERROR(INV_ARG_NUM, tokens, names);
        return NULL;
    }

    return returning;
}

//-------------------------------------------------------------------

Node* _get_label_jump(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    REQUIRE_KEY_WORD(GOTO);

    Node* node = (Node*)node_allocate_memory();
    NODE_INIT_KEY_NODE(node, GOTO_ND);

    Node* label_name = get_id(tokens, names);
    NULL_CHECK(label_name);
    CONNECT_LEFT(node, label_name);

    return node;
}

//-------------------------------------------------------------------

Node* _get_exp(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    if (TOKEN_IS_DEN(CUR_TOKEN))
    {
        tokens->counter++;
        Node* den_nd = (Node*)node_allocate_memory();
        NULL_CHECK(den_nd);

        NODE_INIT_UNAR_OPERAND(den_nd, DEN);
        CONNECT_LEFT(den_nd, get_f(tokens, names));

        return den_nd;
    }
    else
        return get_f(tokens, names);
}

//-------------------------------------------------------------------

Node* _get_f(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* e = get_e(tokens, names);
    NULL_CHECK(e);

    while (TOKEN_IS_CMP_SIGN(CUR_TOKEN))
    {
        Node* f = (Node*)node_allocate_memory();
        NULL_CHECK(f);

        NODE_INIT_BIN_OPERAND(f, CUR_TOKEN->data.cmp_sign_code);
        tokens->counter++;

        CONNECT_LEFT(f, e);

        Node* e2 = get_e(tokens, names);
        NULL_CHECK(e2);

        CONNECT_RIGHT(f, e2);
        e = f;
    }

    return e;
}

//-------------------------------------------------------------------

Node* _get_e(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* t = get_t(tokens, names);
    NULL_CHECK(t);

    while (TOKEN_IS_ADD(CUR_TOKEN) || TOKEN_IS_SUB(CUR_TOKEN))
    {
        Node* e = (Node*)node_allocate_memory();
        NULL_CHECK(e);

        NODE_INIT_BIN_OPERAND(e, (int)(CUR_TOKEN->data.operand));
        tokens->counter++;

        CONNECT_LEFT(e, t);

        Node* t2 = get_t(tokens, names);
        NULL_CHECK(t2);

        CONNECT_RIGHT(e, t2);
        t = e;
    }

    return t;
}

//int _get_cmp_sign(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS));

//-------------------------------------------------------------------

Node* _get_t(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* m = get_m(tokens, names);
    NULL_CHECK(m);

    while (TOKEN_IS_MUL(CUR_TOKEN) || TOKEN_IS_DIV(CUR_TOKEN))
    {
        Node* t = (Node*)node_allocate_memory();
        NULL_CHECK(t);

        NODE_INIT_BIN_OPERAND(t, (int)(CUR_TOKEN->data.operand));
        tokens->counter++;

        CONNECT_LEFT(t, m);

        Node* m2 = get_m(tokens, names);
        NULL_CHECK(m2);

        CONNECT_RIGHT(t, m2);
        m = t;
    }

    return m;
}

//-------------------------------------------------------------------

Node* _get_m(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* p = get_p(tokens, names);
    NULL_CHECK(p);

    if (TOKEN_IS_POW(CUR_TOKEN))
    {
        Node* m = (Node*)node_allocate_memory();
        NULL_CHECK(m);

        NODE_INIT_BIN_OPERAND(m, CUR_TOKEN->data.cmp_sign_code);
        tokens->counter++;

        CONNECT_LEFT(m, p);

        Node* p2 = get_p(tokens, names);
        NULL_CHECK(p2);

        CONNECT_RIGHT(m, p2);
        return m;
    }

    return p;
}

//-------------------------------------------------------------------

Node* _get_calc_func(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* calc_func = (Node*)node_allocate_memory();
    NULL_CHECK(calc_func);

    NODE_INIT_UNAR_OPERAND(calc_func, CUR_TOKEN->data.std_func_code);
    tokens->counter++;

    REQUIRE_KEY_WORD(BR_OPEN);

    Node* exp = get_exp(tokens, names);
    NULL_CHECK(exp);
    CONNECT_LEFT(calc_func, exp);

    REQUIRE_KEY_WORD(BR_CLOSE);

    return calc_func;
}

//-------------------------------------------------------------------

Node* _get_p(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS)) 
{
    SYNTAX_READ_FUNC_START(tokens, names);

    int is_negative = 0;
    if (TOKEN_IS_NEG(CUR_TOKEN))
    {
        is_negative = 1;
        tokens->counter++;
    }

    Node* node = NULL;

    if (TOKEN_IS_BR_OPEN(CUR_TOKEN))
    {
        tokens->counter++;
        node = get_exp(tokens, names);
        NULL_CHECK(node);

        REQUIRE_KEY_WORD(BR_CLOSE);
    }

    else if (TOKEN_IS_ID(CUR_TOKEN) && !TOKEN_IS_BR_OPEN(N_TOKEN))
    {
        node = get_var_id(tokens, names);
        NULL_CHECK(node);
    }

    else if (TOKEN_IS_CONSTANT(CUR_TOKEN))
    {
        node = (Node*)node_allocate_memory();
        NODE_INIT_CONSTANT(node, CUR_TOKEN->data.constant);
        tokens->counter++;
    }

    else if (TOKEN_IS_CALC_FUNC(CUR_TOKEN))
    {

        node = get_calc_func(tokens, names);
        NULL_CHECK(node);
    }

    else
    {
        node = get_func_call(tokens, names);
        NULL_CHECK(node);
    }

    if (is_negative)
    {
        Node* multiply = (Node*)node_allocate_memory();
        NODE_INIT_BIN_OPERAND(multiply, MUL);

        ADD_LEFT(multiply);
        NODE_INIT_CONSTANT(multiply->left_son, -1);
        CONNECT_RIGHT(multiply, node);

        return multiply;
    }

    return node;
}

//===================================================================

Node* _get_var_id(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    int64_t var_hash = CUR_TOKEN->data.id_hash;

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (!var_is_declared(var_hash, names->var_cluster))
    {
        SYNT_ERROR(VAR_UNDECLARED, tokens, names);
        return NULL;
    }

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);

    NODE_INIT_IDENTIFICATOR(node, var_hash);
    tokens->counter++;

    int var_size = get_var_size(var_hash, names->var_cluster);
    RET_VALUE_CHECK(var_size);

    if (TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size == 1)
    {
        SYNT_ERROR(VAR_WITH_INDEX, tokens, names);
        return NULL;
    }

    else if (!TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size > 1)
    {
        SYNT_ERROR(ARRAY_WITH_NO_INDEX, tokens, names);
        return NULL;
    }
    
    else if (TOKEN_IS_SBR_OPEN(CUR_TOKEN) && var_size > 1)
    {
        Node* el_index = get_arr_el_index(tokens, names); //
        NULL_CHECK(el_index);

        CONNECT_RIGHT(node, el_index);
    }

    else
        CONNECT_RIGHT(node, build_index_constr(0));

    return node;
}

//-------------------------------------------------------------------

Node* _get_func_id(Tokens* tokens, Names* names, int* arg_num FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (!func_is_defined(CUR_TOKEN->data.id_hash, names->func_cluster))
    {
        SYNT_ERROR(FUNC_UNDECLARED, tokens, names);
        return NULL;
    }

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);

    NODE_INIT_IDENTIFICATOR(node, CUR_TOKEN->data.id_hash);

    *arg_num = get_func_arg_num(CUR_TOKEN->data.id_hash, names->func_cluster);
    RET_VALUE_CHECK(*arg_num);

    tokens->counter++;

    return node;
}

//-------------------------------------------------------------------

Node* _get_var_id_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    Node* node = NULL;
    Node* decl = NULL;
    int is_permanent = 0;

    if (TOKEN_IS_PERM(CUR_TOKEN))
    {
        node = (Node*)node_allocate_memory();
        NULL_CHECK(node);

        NODE_INIT_KEY_NODE(node, PERM_ND);
        tokens->counter++;
        
        ADD_RIGHT(node);
        NULL_CHECK(node->R);

        decl = node;
        node = node->R;
        is_permanent = 1;
    }

    else
    {
        node = (Node*)node_allocate_memory();
        NULL_CHECK(node);
    }

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (var_is_declared(CUR_TOKEN->data.id_hash, names->var_cluster))
    {
        SYNT_ERROR(VAR_REDECLARE, tokens, names);
        return NULL;
    }

    else 
        add_var_declare(CUR_TOKEN->data.id_hash, is_permanent, 1, 
                                             names->var_cluster);

    NODE_INIT_IDENTIFICATOR(node, CUR_TOKEN->data.id_hash);
    tokens->counter++;

    ADD_RIGHT(node);
    NODE_INIT_KEY_NODE(NR, SIZE_ND);
    
    ADD_RIGHT(NR);
    NODE_INIT_CONSTANT(NRR, 1);

    return (is_permanent)? decl: node;
}

//-------------------------------------------------------------------

Node* _get_func_id_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    int64_t func_hash = 0;

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (func_is_defined(CUR_TOKEN->data.id_hash, names->func_cluster))
    {
        SYNT_ERROR(FUNC_REDECL, tokens, names);
        return NULL;
    }

    else
        func_hash = CUR_TOKEN->data.id_hash;

    Node* func_nd = (Node*)node_allocate_memory();
    NULL_CHECK(func_nd);
    NODE_INIT_KEY_NODE(func_nd, FUNC_ND);

    ADD_LEFT(func_nd);
    NODE_INIT_IDENTIFICATOR(func_nd->L, func_hash);

    tokens->counter++;

    int arg_num = 0;

    Node* parameters = get_func_parameters(&arg_num, tokens, names);
    NULL_CHECK(parameters);
    CONNECT_RIGHT(func_nd, parameters);

    int ret = add_func_defn(func_hash, arg_num, names->func_cluster);
    RET_VALUE_CHECK(ret);

    return func_nd;      
} 

//-------------------------------------------------------------------

Node* _get_label_id_decl(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        SYNT_ERROR(MISSING_ID, tokens, names);
        return NULL;
    }

    else if (label_is_defined(CUR_TOKEN->data.id_hash, names->label_cluster))
    {
        SYNT_ERROR(LABEL_REDECL, tokens, names);
        return NULL;
    }

    else
        add_label_defn(CUR_TOKEN->data.id_hash, names->label_cluster);

    Node* node = (Node*)node_allocate_memory();
    NULL_CHECK(node);

    NODE_INIT_IDENTIFICATOR(node, CUR_TOKEN->data.id_hash);
    tokens->counter++;

    return node;
}

//===================================================================

Node* _get_id(Tokens* tokens, Names* names FOR_LOGS(, LOG_PARAMS))
{
    SYNTAX_READ_FUNC_START(tokens, names);

    if (!TOKEN_IS_ID(CUR_TOKEN))
    {
        error_report(MISSING_ID);
        tokens_dump(tokens, logs_file);
        return NULL;
    }

    Node* id = (Node*)node_allocate_memory();
    NULL_CHECK(id);

    NODE_INIT_IDENTIFICATOR(id, CUR_TOKEN->data.id_hash);
    tokens->counter++;

    return id;
}

//===================================================================

#undef N_TOKEN
#undef CUR_TOKEN
#undef N_N_TOKEN
#undef PR_TOKEN

//-------------------------------------------------------------------

#undef NL
#undef NR
#undef NLL
#undef NRR
#undef NLR
#undef NRL

//===================================================================
