#pragma once

#include "../diff/diff.h"

//===================================================================

#define WRITE_NODE_IN_BRACKETS(node, output) {                        \
                                                                      \
       do                                                             \
       {                                                              \
              fprintf(tex, "{");                                      \
              node_write_latex(node, output);                         \
              fprintf(tex, "}");                                      \
                                                                      \
       } while(0);                                                    \
}

//===================================================================

#define tree_latex_original_expr(tree, tex) \
       _tree_latex_execute(tree, tex, "Original expression before simplifying", LOG_ARGS)

#define tree_latex_original_after_simp(tree, tex) \
       _tree_latex_execute(tree, tex, "Receiving signals from space, we get " \
                                      "an intermediate result after simplification", LOG_ARGS)

#define latex_show_derivative(diff, tex) \
       _tree_latex_execute(diff, tex, "Derivative is: ", LOG_ARGS)

//===================================================================

int  _tree_latex_execute(struct Tree* tree, FILE* tex, const char* phrase, 
                                                              LOG_PARAMS);

FILE*  _tree_latex_start(struct Tree* tree, LOG_PARAMS);

int _tree_latex_add_conspect(struct Tree* tree, FILE* tex, LOG_PARAMS);

int _tree_latex_finish(struct Tree* tree, FILE* tex, const char* tex_name, LOG_PARAMS);

int _latex_write_title(FILE* tex, LOG_PARAMS);

int are_brackets_needed(struct Node* node);

FILE* _open_latex_file(const char* tex_name, LOG_PARAMS);

int _call_latex(const char* tex_name, LOG_PARAMS);

int _node_write_latex(struct Node* node, FILE* tex, LOG_PARAMS);

int _print_latex_data(struct Node* node, FILE* tex, LOG_PARAMS);

int _print_latex_operand(struct Node* node, FILE* tex, LOG_PARAMS);

//===================================================================

#define print_latex_operand(node, tex) \
       _print_latex_operand(node, tex, LOG_ARGS)

#define node_write_latex(node, tex) \
       _node_write_latex(node, tex, LOG_ARGS)

#define print_latex_data(node, tex) \
       _print_latex_data(node, tex, LOG_ARGS)

#define call_latex(tex_dest) \
       _call_latex(tex_dest, LOG_ARGS);

#define open_latex_file(tex_name) \
       _open_latex_file(tex_name, LOG_ARGS)

#define latex_write_title(tex) \
       _latex_write_title(tex, LOG_ARGS)

#define tree_latex_execute(tree, tex, phrase) \
       _tree_latex_execute(tree, tex, phrase, LOG_ARGS)

#define tree_latex_start(tree) \
       _tree_latex_start(tree, LOG_ARGS)    

#define tree_latex_add_conspect(tree, tex) \
       _tree_latex_add_conspect(tree, tex, LOG_ARGS)

#define tree_latex_finish(tree, tex, tex_dest) \
       _tree_latex_finish(tree, tex, tex_dest, LOG_ARGS)
