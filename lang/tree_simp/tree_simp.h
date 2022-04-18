#pragma once

#include "../tree/tree.h"

//===================================================================

int _tree_simplify(struct Tree* tree FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define TREE_SIMPLIFY(tree) {                                       \
                                                                    \
    do                                                              \
    {                                                               \
        int is_simp = 0;                                            \
        do                                                          \
        {                                                           \
            is_simp = tree_simplify(tree);                          \
            if (is_simp == -1)                                      \
                return -1;                                          \
                                                                    \
        } while (is_simp == 1);                                     \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define DESTRUCT_BRANCH(node) {                                     \
                                                                    \
    do                                                              \
    {                                                               \
        int is_destructed = node_visiter(node, _node_destruct);     \
        if (is_destructed == -1)                                    \
            return -1;                                              \
    } while(0);                                                     \
}                                   


//===================================================================

#define calc_bin_operand_value(node) \
       _calc_bin_operand_value(node FOR_LOGS(, LOG_ARGS))

#define calc_unar_operand_value(node) \
       _calc_unar_operand_value(node FOR_LOGS(, LOG_ARGS))

#define tree_simplify(diff) \
       _tree_simplify(diff FOR_LOGS(, LOG_ARGS))

#define constant_folding(node) \
       _constant_folding(node FOR_LOGS(, LOG_ARGS))

#define cut_nodes(diff, node) \
       _cut_nodes(diff, node FOR_LOGS(, LOG_ARGS))

#define is_zero_node(node) \
       _is_zero_node(node FOR_LOGS(, LOG_ARGS))

#define is_one_node(node) \
       _is_one_node(node FOR_LOGS(, LOG_ARGS))

#define replace_with_const(node, constant) \
       _replace_with_const(node, constant FOR_LOGS(, LOG_ARGS))

#define cut_constant(diff, node, cutted) \
       _cut_constant(diff, node, cutted FOR_LOGS(, LOG_ARGS))  

#define mul_by_one_simp_check(node) \
       _mul_by_one_simp_check(node FOR_LOGS(, LOG_ARGS))

#define mul_by_zero_simp_check(node) \
       _mul_by_zero_simp_check(node FOR_LOGS(, LOG_ARGS))
       
#define sum_with_zero_simp_check(node) \
       _sum_with_zero_simp_check(node FOR_LOGS(, LOG_ARGS))

#define pow_zero_simp_check(node) \
       _pow_zero_simp_check(node FOR_LOGS(, LOG_ARGS))

#define pow_one_simp_check(node) \
       _pow_one_simp_check(node FOR_LOGS(, LOG_ARGS))
