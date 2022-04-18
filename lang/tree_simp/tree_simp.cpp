#include <math.h>

#include "tree_simp.h"
#include "../general/general.h"

//===================================================================

static const double Pi = 3.141593;

//===================================================================

static int _constant_folding(struct Node* node FOR_LOGS(, LOG_PARAMS));

static int _cut_nodes(struct Tree* tree, struct Node** node FOR_LOGS(, LOG_PARAMS));

static Node* _is_zero_node(struct Node* node FOR_LOGS(, LOG_PARAMS));

static Node* _is_one_node(struct Node* node FOR_LOGS(, LOG_PARAMS));

static int _replace_with_const(struct Node* node, double constant FOR_LOGS(, LOG_PARAMS));

static int _cut_constant(struct Tree* tree, struct Node** node, struct Node* cutted, 
                                                                        LOG_PARAMS);

static int _calc_bin_operand_value(Node* node FOR_LOGS(, LOG_PARAMS));

static int _calc_unar_operand_value(Node* node FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define L left_son

#define R right_son

//===================================================================

int _tree_simplify(struct Tree* tree FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();              
    TREE_PTR_CHECK(tree);

    int constant_simp = constant_folding(tree->root);
    if (constant_simp == -1)
        return -1;

    int cut_nodes = cut_nodes(tree, &(tree->root));
    if (cut_nodes == -1)
        return -1;

    //tree_draw_graph(tree);

    return (constant_simp || cut_nodes );
}

//===================================================================

static int _calc_bin_operand_value(Node* node FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NODE_PTR_CHECK(node);
    
    double left  = node->L->data.constant;
    double right = node->R->data.constant;
    double res   = 0;

    switch(node->data.operand_code)
    {
        case ADD:  res = left + right;                  break;

        case SUB:  res = left - right;                  break;

        case MUL:  res = left * right;                  break;

        case DIV:  res = left / right;                  break;

        case MORE: res = (left > right);                break;

        case LESS: res = (left < right);                break;

        case POW:  res = pow(left, right);              break;

        case EQ:   res = double_is_equal(left, right);  break;

        case NEQ:  res = !double_is_equal(left, right); break;

        case MEQ:  res = (left > right) || (double_is_equal(left, right)); break;

        case LEQ:  res = (left < right) || (double_is_equal(left, right)); break;

        default: 
        {
            error_report(INV_OPER_CODE);
            return -1;
        }
    }

    DESTRUCT_BRANCH(node->L);
    DESTRUCT_BRANCH(node->R);

    node->L = NULL;
    node->R = NULL;

    ADD_CONSTANT_NODE(node, res);

    return 1;
}

//-------------------------------------------------------------------

static int _calc_unar_operand_value(Node* node FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    NODE_PTR_CHECK(node);

    double arg = node->L->data.constant;
    double res = 0;

    switch (node->data.operand_code)
    {
        case SIN   : res = sin(arg);         break;

        case COS   : res = cos(arg);         break;

        case TG    :  res = tan(arg);        break;

        case CTG   : res = 1 / tan(arg);     break;

        case LN    : res = log(arg);         break;

        case ARCCOS: res = acos(arg);        break;

        case ARCSIN: res = asin(arg);        break;

        case ARCTG : res = atan(arg);        break;

        case ARCCTG: res = Pi/2 - atan(arg); break;

        case SQRT  : res = sqrt(arg);        break;

        case SQR   : res = pow(arg, 2);      break;

        case DEN   : res = (arg <= 0)? 1: 0; break;

        default:
        {
            error_report(INV_OPER_CODE);
            return -1;
        }
    }  

    DESTRUCT_BRANCH(node->L);
    node->L = NULL;

    ADD_CONSTANT_NODE(node, res);

    return 1;
}

//===================================================================

static int _constant_folding(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    NODE_PTR_CHECK(node);

    int ret = 0;
    int is_simplified = 0;

    if (node->L) {

        ret = constant_folding(node->L);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if (node->R) {

        ret = constant_folding(node->R);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if (node->data_type    == BIN_OPERAND
    &&  node->L->data_type == CONSTANT
    &&  node->R->data_type == CONSTANT)
    {
        ret = calc_bin_operand_value(node);
        RETURN_CHECK(ret);

        is_simplified++;
    }

    if (node->data_type   == UNAR_OPERAND
    && node->L->data_type == CONSTANT)
    {
        ret = calc_unar_operand_value(node);
        RETURN_CHECK(ret);

        is_simplified++;
    }

    return is_simplified;
}

//===================================================================

static Node* _is_zero_node(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    if (!node)
        return NULL;

    if (node->L
     && node->data_type == CONSTANT
     && double_is_equal(node->L->data.constant, 0))
        return node->L;
    
    if (node->R
     && node->data_type == CONSTANT
     && double_is_equal(node->R->data.constant, 0))
        return node->R;

    return NULL;
}

//===================================================================

static Node* _is_one_node(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    if (!node)
        return NULL;

    if (node->L
     && node->data_type == CONSTANT
     && double_is_equal(node->L->data.constant, 1))
        return node->L;
    
    if (node->R
     && node->data_type == CONSTANT
     && double_is_equal(node->R->data.constant, 1))
        return node->R;

    return NULL;
}

//===================================================================

static int _replace_with_const(struct Node* node, double constant FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    NODE_PTR_CHECK(node);

    DESTRUCT_BRANCH(node->L);
    DESTRUCT_BRANCH(node->R);

    node->L = NULL;
    node->R = NULL;

    ADD_CONSTANT_NODE(node, constant);

    return 1;
}

//===================================================================

static int _cut_constant(struct Tree* tree, struct Node** node_ptr, struct Node* cutted, 
                                                                             LOG_PARAMS) {

    lang_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(tree);

    struct Node* node = *node_ptr;
    struct Node* expression = NULL;

    if (node->L != cutted)
        expression = node->L;
    else 
        expression = node->R;

    *node_ptr = expression;
    (*node_ptr)->parent = node->parent;

    if (node->parent == No_parent)
        tree->root = expression;

    else if (node == node->parent->left_son)
        node->parent->left_son  = expression;
        
    else if (node == node->parent->right_son)
        node->parent->right_son = expression;

    DESTRUCT_BRANCH(cutted);
    int ret = node_destruct(node);
    if (ret == -1)
        return -1;

    return 1;
}

//===================================================================

static int _mul_by_zero_simp_check(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    NODE_PTR_CHECK(node)

    if (node->data_type         == BIN_OPERAND 
    &&  node->data.operand_code == MUL 
    &&  is_zero_node(node))

        return 1;
    else 
        return 0;
}

//-------------------------------------------------------------------

static Node* _mul_by_one_simp_check(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if  (node->data_type         == BIN_OPERAND
    &&   node->data.operand_code == MUL 
    &&  (returning = is_one_node(node)))

        return returning;
    else 
        return NULL;
}

//-------------------------------------------------------------------

static Node* _sum_with_zero_simp_check(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (node->data_type         == BIN_OPERAND
    && (node->data.operand_code == ADD
    ||  node->data.operand_code == SUB)
    && (returning = is_zero_node(node)))

        return returning;
    else
        return NULL;
}

//-------------------------------------------------------------------

static Node* _pow_one_simp_check(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (node->data_type         == BIN_OPERAND
    &&  node->data.operand_code == POW
    && (returning = is_one_node(node)))

        return returning;
    else
        return NULL;
}

//-------------------------------------------------------------------

static int _pow_zero_simp_check(struct Node* node FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    NODE_PTR_CHECK(node);

    if (node->data_type    == BIN_OPERAND
    &&  node->data.operand_code == POW
    &&  is_zero_node(node))

        return 1;
    else
        return 0;
}

//===================================================================

static int _cut_nodes(struct Tree* tree, struct Node** node_ptr FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(tree);

    struct Node* node = *node_ptr;

    int is_simplified = 0;
    int ret = 0;
    struct Node* cutted = NULL;

    if (node->L) {

        ret = cut_nodes(tree, &(node->L));
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if (node->R) {

        ret = cut_nodes(tree, &(node->R));
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if (mul_by_zero_simp_check(node)) {

        ret = replace_with_const(node, 0);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if ((cutted = mul_by_one_simp_check(node))) {

        ret = cut_constant(tree, &node, cutted);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if ((cutted = sum_with_zero_simp_check(node))) {

        ret = cut_constant(tree, &node, cutted);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    if (pow_zero_simp_check(node)) {

        ret = replace_with_const(node, 1);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;

    }

    if ((cutted = pow_one_simp_check(node))) {

        ret = cut_constant(tree, &node, cutted);
        RETURN_CHECK(ret);

        if (ret) is_simplified++;
    }

    return is_simplified;
}

//===================================================================

#undef R
#undef L