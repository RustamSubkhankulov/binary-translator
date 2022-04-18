#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "diff_tex.h"
#include "../general/general.h"

//===================================================================

const char* Tex_name = "diff.tex";

#define R right_son

#define L left_son

//===================================================================

int  _tree_latex_execute(struct Tree* tree, FILE* tex, const char* phrase, 
                                                               LOG_PARAMS) {

    TREE_PTR_CHECK(tree);
    if (tex == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(tex, "\n\n\n \\textbf{%s:}\n\n", phrase);
    fprintf(tex, "\\begin{math}\n");

    int ret = node_write_latex(tree->root, tex);
    if (ret == -1)
        return -1;

    fprintf(tex, "\n\\end{math} \n \\\\ \n\n");
    return 0;
}

//===================================================================

int _print_latex_operand(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    switch (node->data.operand) {

        case ADD:  fprintf(tex, "+");         break;

        case SUB:  fprintf(tex, "-");         break;

        case MUL:  fprintf(tex, " \\cdot ");  break;

        case DIV:  fprintf(tex, " \\frac ");  break;

        case POW:  fprintf(tex, "^");         break;

        case SIN:  fprintf(tex, "\\sin");     break;

        case COS:  fprintf(tex, "\\cos");     break;

        case  TG:  fprintf(tex, "\\tan");     break;

        case CTG:  fprintf(tex, "\\cot");     break;

        case ACOS: fprintf(tex, "\\arccos");  break;

        case ASIN: fprintf(tex, "\\arrcsin"); break;

        case  ATG: fprintf(tex, "\\arctan");  break;

        case ACTG: fprintf(tex, "\\arccot");  break;

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }

    return 0;
}

//===================================================================

int _print_latex_data(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) {

        case CONSTANT: {

            fprintf(tex, "%g", node->data.constant);
            break;
        }

        case VARIABLE: {

            fprintf(tex, "%c", node->data.variable);
            break;
        }

        case OPERAND: {

            return print_latex_operand(node, tex);
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

static int operand_priority(struct Node* node) {

    if (node->data.operand == ADD
    ||  node->data.operand == SUB)

        return 2;

    if (node->data.operand == MUL
    ||  node->data.operand == DIV
    ||  node->data.operand == POW)

        return 1;

    return 0;
}

//===================================================================

int are_brackets_needed(struct Node* node) {

    struct Node* parent = node->parent;
    struct Node* left   = node->left_son;
    struct Node* right  = node->right_son;

    if ((!left && !right && !is_function_operand(parent))
    ||   (IS_OPER(node)
    &&   (is_function_operand(node) 
    ||   node->data.operand == DIV)))

        return 1;
    
    if (IS_OPER(node)
    &&  parent != No_parent
    &&  IS_OPER(parent)
    &&  operand_priority(node) 
    ==  operand_priority(parent))

        return 1;

    if (parent == No_parent
    || (parent != No_parent 
    &&  IS_OPER(parent)
    &&  parent->data.operand == DIV))

        return 1;

    if  (IS_OPER(node)
    &&  (is_function_operand(node)
    ||  operand_priority(node) == 1)
    &&  parent != No_parent 
    &&  IS_OPER(parent)
    &&  operand_priority(parent) == 2)

        return 1;

    if (IS_OPER(node)
    &&  node->data.operand == POW)

        return 1;

    return 0;
}

//===================================================================

int _node_write_latex(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    int no_brackets = are_brackets_needed(node);

    int children_in_brackets = 0;

    if (IS_OPER(node)
    &&  node->data.operand == DIV)

        children_in_brackets = 1;

    if (!no_brackets) 
        fprintf(tex, "(");

    if (IS_OPER(node) 
    && (is_function_operand(node) 
    ||  node->data.operand == DIV))
    
        print_latex_data(node, tex);

    if (node->L) {

        if (children_in_brackets)
            WRITE_NODE_IN_BRACKETS(node->L, tex)
        else
            node_write_latex(node->L, tex);
    }

    if (!IS_OPER(node)
    || ( IS_OPER(node) 
    &&  node->data.operand != DIV 
    && !is_function_operand(node)))

        print_latex_data(node, tex);

    if (node->R) {

        if (children_in_brackets)
            WRITE_NODE_IN_BRACKETS(node->R, tex)
        else
            node_write_latex(node->R, tex);
    }

    if (!no_brackets)
        fprintf(tex, ")");

    return 0;
}

//===================================================================

int _latex_write_title(FILE* tex, LOG_PARAMS) {

    diff_log_report();

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(tex, "\\documentclass[a4paper, 12pt] {article}\n");
    fprintf(tex, "\\usepackage{graphicx}\n");
    fprintf(tex, "\\usepackage[utf8]{inputenc}\n");
    fprintf(tex, "\\usepackage{multirow}\n");
    fprintf(tex, "\\usepackage{mathtext}\n");
    fprintf(tex, "\\usepackage[T2A]{fontenc}\n");
    fprintf(tex, "\\usepackage{titlesec}\n");
    fprintf(tex, "\\usepackage{float}\n");
    fprintf(tex, "\\usepackage{empheq}\n");
    fprintf(tex, "\\usepackage{amsfonts}\n");
    fprintf(tex, "\\usepackage{amsmath}\n");
    fprintf(tex, "\\title{Semester work in mathematical analysis}\n");
    fprintf(tex, "\\author{Rustam Subkhankulov \\thanks{sponsored By A.Y.Petrovich}}\n");
    fprintf(tex, "\\date{November 2021}\n");
    fprintf(tex, "\\begin{document} \n");
    fprintf(tex, "\\maketitle\n");
    fprintf(tex, "\\newpage\n");

    return 0;
}

//===================================================================

FILE* _open_latex_file(const char* tex_name, LOG_PARAMS) {

    diff_log_report();

    char dir_name_buffer[Directory_name_buffer_size] = { 0 };
    sprintf(dir_name_buffer, "%s%s", LATEX_DIR, tex_name);

    return open_file(dir_name_buffer, "wb");
}

//===================================================================

FILE* _tree_latex_start(struct Tree* tree, LOG_PARAMS) {

    FILE* tex = open_latex_file(Tex_name);
    if (!tex)
        return NULL;

    int ret = latex_write_title(tex);
    if (ret == -1)
        return NULL;

    return tex;
}

//===================================================================

int _tree_latex_add_conspect(struct Tree* tree, FILE* tex, LOG_PARAMS) {

    TREE_PTR_CHECK(tree);
    if (!tex)
        return -1;

    srand((unsigned)clock());

    int random__first  = rand() % 10;
    int random__second = rand() % 100;

    fprintf(tex, "\n\n\n \\textbf{Using %d.%d theorema"
                 " we will have that derivative is:}\n\n", 
                                            random__first, 
                                          random__second);

    fprintf(tex, "\\begin{math}\n");

    int ret = node_write_latex(tree->root, tex);
    if (ret == -1)
        return -1;

    fprintf(tex, "\n\\end{math} \\\\ \n");

    fprintf(tex, "\n");


    return 0;
}

//===================================================================

int _call_latex(const char* tex_dest, LOG_PARAMS) {

    diff_log_report();

    char buffer[System_cmnd_buf_size] = { 0 };
    sprintf(buffer, "pdflatex %s%s -output-directory=%s%s >/dev/null", 
                                                  LATEX_DIR, Tex_name, 
                                                  LATEX_DIR, tex_dest);
    return system(buffer);
}

//===================================================================

int _tree_latex_finish(struct Tree* tree, FILE* tex, const char* tex_dest, 
                                                               LOG_PARAMS) {

    TREE_PTR_CHECK(tree);

    if (!tex_dest) {

        error_report(INV_FILE_NAME);
        return -1;
    }

    fprintf(tex, "\n\n\\textbf{Results after diffferentiation and simplification:}");
    fprintf(tex, "\n \\begin{math}\n");

    int ret = node_write_latex(tree->root, tex);
    if (ret == -1)
        return -1;

    fprintf(tex, "\n \\end{math}\n");
    fprintf(tex, "\\end{document}\n");

    ret = close_file(tex);
    if (ret == -1)
        return -1;

    ret = call_latex(tex_dest);
    if (ret != 0)
        return -1;

    return 0;    
}