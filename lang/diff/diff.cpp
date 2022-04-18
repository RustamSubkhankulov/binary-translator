#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "diff.h"
#include "diff_tex.h"
#include "diff_variables.h"
#include "../text_processing/text_processing.h"
#include "../general/general.h"

//===================================================================

static int _read_closing_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS);

static int _read_opening_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS);

static int  _diff_operand_node(struct Node* orig, struct Node* diff, char var, LOG_PARAMS);

static int _diff_operand_add_or_sub(struct Node* orig, struct Node* diff, int oper, char var,
                                                                                  LOG_PARAMS);

static int _diff_operand_mul(struct Node* orig, struct Node* diff, char var, LOG_PARAMS);

static int _diff_operand_div(struct Node* orig, struct Node* diff, char var, LOG_PARAMS);

static int _diff_operand_pow(struct Node* orig, struct Node* diff, char var, LOG_PARAMS);

static int _diff_operand_sin(struct Node* orig, struct Node* diff,  char var, LOG_PARAMS);

static int _diff_operand_cos(struct Node* orig, struct Node* diff,  char var, LOG_PARAMS);

static int _diff_operand_tg (struct Node* orig, struct Node* diff,  char var, LOG_PARAMS);

static int _diff_operand_ctg(struct Node* orig, struct Node* diff,  char var, LOG_PARAMS);

static int _node_is_constant(struct Node* node, char var, LOG_PARAMS);

static int _complex_diff_add_sum(struct Node** diff_dest, struct Node** sum_node,  
                                                   struct Tree* diff, LOG_PARAMS);

static int _diff_operand_arcsin_and_arccos(struct Node* orig, struct Node* diff, 
                                               int oper,  char var, LOG_PARAMS);

static int _diff_operand_arctg_and_arcctg(struct Node* orig, struct Node* diff, 
                                              int oper,  char var, LOG_PARAMS);

static char _get_var_diff_by(LOG_PARAMS);

//===================================================================

#define L left_son

#define R right_son

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    int ctor_ret = tree_ctor(tree);
    if (ctor_ret == -1)
        return -1;

    NODE_INIT_CONSTANT(tree->root, 0);
    tree->root->parent = No_parent;

    DIFF_TREE_VERIFICATION(tree);

    return 0;
}

//===================================================================

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    return tree_dtor(tree);
}

//===================================================================

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS) {

    diff_log_report();

    if (!tree) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    int size = 0;

    char* buffer = copy_from_file_to_buffer(filename, &size);
    if (buffer == NULL)
        return NULL;

    struct Buffer_struct buffer_struct = { 0 };
    buffer_struct_init(&buffer_struct, buffer, size, 0);

    int ret = node_read_from_buffer(tree->root, &buffer_struct);
    if (ret == -1) {

        free(buffer);
        return NULL;
    }

    int is_ok = tree_validator(tree);
    if (is_ok == -1)
        return NULL;
    
    return buffer;
}

//===================================================================

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    const char* buffer = buffer_struct->buffer;
    int size           = buffer_struct->size;
    int pos            = buffer_struct->pos;

    if (buffer == NULL) {

        error_report(INV_BUF_PTR);
        return -1;
    }

    fprintf(logs_file, "<style> td\n { padding: 7px;"
                                  " border: 1px solid black;"
                                  " border-collapse: collapse;}\n</style>");

    fprintf(logs_file, "<table width = \" 100%% \" "
                                  " cellspacing=\"0\" "
                                  " cellpadding=\"4\" "
                                  " border = \"5\" "
                                  " style = \" "
                                  " padding: 15px; "
                                  " background-color: lightgrey;>\"\n");

    int counter = 0;
    while (counter < size) {

        int second_counter = counter;
        fprintf(logs_file, "\n<tr><td style = \"background-color: #c2e7c2\"><b>Index</b></td>");

        for(int ct = 0; ct < 16 && second_counter < size; ct++, second_counter++) {

            if (second_counter == pos)
                fprintf(logs_file, "<td style = \"background-color: green;"
                                                 "color: white;\"> %d </td>", 
                                                             second_counter);
            else
                fprintf(logs_file,"<td style = \"background-color: #c2e7c2\"> %d </td>", 
                                                                        second_counter);
        }

        fprintf(logs_file, "</tr><tr><td><b> Symbol </b></td>");

        for (int ct = 0; ct < 16 && counter < size; ct++, counter++) {

            if (counter == pos)
                fprintf(logs_file, "<td style = \"background-color: green;"
                                                 "color: white;\">");
            else 
                fprintf(logs_file, "<td>");

            if (isblank(buffer[counter]) || buffer[counter] == 0)
                fprintf(logs_file, " %d </td> ", buffer[counter]);
            else
                fprintf(logs_file, " %c </td> ", buffer[counter]);
        }

        fprintf(logs_file, "</tr>");

    }

    fprintf(logs_file, "</table> \n");
    fflush(logs_file);

    return 0;
}

//===================================================================

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                            int size, int pos, LOG_PARAMS)  {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    if (buffer == NULL) {

        error_report(INV_BUF_PTR);
        return -1;
    }

    buffer_struct->buffer = buffer;
    buffer_struct->size   = size;
    buffer_struct->pos    = pos;

    return 0;
}

//===================================================================

static int _read_function(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int prev_pos = buffer_struct->pos;

    char buffer[Function_name_buf_size] = { 0 };
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %s %n", 
                                                                buffer, &offset);
    if (!scanned) {

        error_report(TEXT_PROCESSING_ERR);
        return -1;
    }

    int64_t hash = get_hash(buffer, strlen(buffer));

    for (int counter = 0; counter < Functions_number; counter++) {

        if (hash == Functions[counter].hash) {

            NODE_INIT_OPERAND(node, Functions[counter].code);

            buffer_struct->pos += offset;

            ADD_LEFT(node);

            NODE_READ_FROM_BUFFER(node->L, buffer_struct);
            READ_CLOSING_BRACKET(buffer_struct);
            
            return 1;
        }
    }

    buffer_struct->pos = prev_pos;
    return 0;
}

//===================================================================

static int _read_constant(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    double constant = 0;
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %lf %n", 
                                                            &constant, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TEXT_PROCESSING_ERR);
        buffer_dump(buffer_struct);
        return -1;
    }

    buffer_struct->pos += offset;

    return node_init_constant(node, constant);
}

//===================================================================

static int _read_node_data(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    char symb = 0;
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                              &symb, &offset);

    switch (scanned) {

        case -1: {

            error_report(TEXT_PROCESSING_ERR);
            buffer_dump(buffer_struct);
            return -1;
        }

        case 1: {

            if (symb_is_var_name(symb)) {

                NODE_INIT_VARIABLE(node, symb);

                if (check_new_var(symb))
                    add_new_var(symb);

                buffer_struct->pos += offset;
            }

            else if (symb_is_operand((int)symb)) {

                NODE_INIT_OPERAND(node, (int)symb);
                buffer_struct->pos += offset;
            }

            else {

                int ret = read_constant(node, buffer_struct);
                if (ret == -1)
                    return -1;

                break;  
            }

            break;
        }

        default: {

            error_report(TREE_TEXT_INV_SYNTAXIS);
            return -1;
        }
    }

    return 0;
}

//===================================================================

static int _read_node_with_children(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                                LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    node->data_type = OPERAND;

    ADD_LEFT_AND_RIGHT(node);

    int ret = node_read_from_buffer(node->left_son, buffer_struct);
    if (ret == -1)
        return -1;

    ret = read_node_data(node, buffer_struct);
    if (ret == -1)
        return -1;

    ret = node_read_from_buffer(node->right_son, buffer_struct);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

static int _read_opening_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    diff_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                          &symb, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    if (symb == '(') {

        buffer_struct->pos += offset;
        return 0;
    }

    else {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    } 

    return 0;

}

//===================================================================

static int _read_closing_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    diff_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                          &symb, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    if (symb == ')') {

        buffer_struct->pos += offset;
        return 0;
    }

    else {

        error_report(TREE_TEXT_NO_CLOSING_BRACKET);
        buffer_dump(buffer_struct);
        return -1;
    } 

    return 0;
}

//===================================================================

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int open_bracket = read_opening_bracket(buffer_struct);
    if (open_bracket == -1)
        return -1;

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                              &symb, &offset);
    
    if (scanned == -1) {

        error_report(TEXT_PROCESSING_ERR);
        buffer_dump(buffer_struct);
        return -1;
    }

    int scan_func = read_function(node, buffer_struct);
    if (scan_func == -1)
        return -1;
    if (scan_func == 1)
        return 1;

    if (scanned == 0 || symb != '(') {

        int ret = read_node_data(node, buffer_struct);
        if (ret == -1)
            return -1;
    }

    else if (symb == '(') {

        int ret = read_node_with_children(node, buffer_struct);
        if (ret == -1)
            return -1;
    }

    else {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    return read_closing_bracket(buffer_struct);
}

//===================================================================

static const char* _input_skip_blanks(char* buffer, LOG_PARAMS) {

    diff_log_report();

    char* slider = &buffer[strlen(buffer) - 1];

    while (isblank(*slider) || *slider == '\n')
        slider--;

    *(slider + 1) = '\0';

    const char* prepared_input = buffer;

    while (isblank(*prepared_input))
        prepared_input++;

    return prepared_input;
}

//===================================================================

static char* _diff_scan_input(LOG_PARAMS) {

    diff_log_report();

    char buf[Console_input_buf_size] = { 0 };

    char* scanned = fgets(buf, Console_input_buf_size, stdin);
    if (scanned == NULL) {

        error_report(CONSOLE_READ_ERROR);
        return NULL;
    }

    buf[strlen(buf) - 1] = '\0';

    while (strlen(buf) == 0) {

        clean_buffer(buf, Console_input_buf_size);

        printf("Error occurred during reading. Please try again.\n");

        fgets(buf, Console_input_buf_size, stdin);
        buf[strlen(buf) - 1] = '\0';
    }

    const char* prepared = input_skip_blanks(buf);
    return strdup(prepared);
}

//===================================================================

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    if (tree == NULL) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    if (!tree) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    char* buffer = diff_scan_input();
    if (buffer == NULL)
        return NULL;

    struct Buffer_struct buffer_struct = { 0 };
    buffer_struct_init(&buffer_struct, buffer, (int)strlen(buffer), 0);

    int ret = node_read_from_buffer(tree->root, &buffer_struct);
    if (ret == -1) {

        free(buffer);
        return NULL;
    }

    int is_ok = tree_validator(tree);
    if (is_ok == -1)
        return NULL;

    return buffer;   
}

//===================================================================

int _diff_copy_branch(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    diff->data      = orig->data;
    diff->data_type = orig->data_type;

    if (orig->left_son) {

        int ret = node_add_left_son(diff);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig->left_son, diff->left_son);
        if (ret == -1)
            return -1;
    }

    if (orig->right_son) {

        int ret = node_add_right_son(diff);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig->right_son, diff->right_son);
        if (ret == -1)
            return -1;
    }

    return 0;
}

//===================================================================

static char _get_var_diff_by(LOG_PARAMS) {

    diff_log_report();

    printf("\n\n Choose a variable: \n\n");

    printf("\n 0). all ");
    print_vars(stdout);
    putchar('\n');

    int var_number = 0;
    int scanned    = 0;

    while ((scanned = scanf(" %d", &var_number)) != 1
       && (var_number < 0 || var_number > get_vars_number())) {

        if (scanned == -1) {

            error_report(INPUT_ERR);
            return -1;
        }

        printf("\n\n Please, try again.\n\n");
        clearstdin();
    }
    if (!var_number)
        return 0;
    else
        return get_var_by_number(var_number - 1);
}

//===================================================================

static int _complex_diff_add_sum(struct Node** diff_dest, struct Node** sum_node,  
                                                   struct Tree* diff, LOG_PARAMS) {

    NODE_PTR_CHECK(diff_dest);
    NODE_PTR_CHECK(sum_node);
    TREE_PTR_CHECK(diff);

    struct Node* prev_sum = *sum_node;

    *sum_node = (Node*)node_allocate_memory();
    if (!(*sum_node))
        return -1;

    (*sum_node)->parent = No_parent;
    diff->root = *sum_node;

    NODE_INIT_OPERAND (*sum_node, ADD);

    (*sum_node)->left_son  = prev_sum;
    prev_sum->parent       = *sum_node;

    ADD_RIGHT(*sum_node);
    *diff_dest = (*sum_node)->right_son;
    (*sum_node)->right_son->parent = (*sum_node);

    return 0;
}

//===================================================================

int _diff_execute_single(struct Tree* tree, struct Tree* diff, FILE* tex, 
                                                    char var, LOG_PARAMS) {
    diff_log_report();

    NODE_DIFF(tree->root, diff->root, var);
    tree_draw_graph(diff);

    fprintf(tex, "\n \\textbf{Differentiation by %c}\n", var);
    latex_show_derivative (diff, tex);
    TREE_SIMPLIFY_WITH_TEX(diff, tex);

    return 0;
}

//===================================================================

int _diff_execute_all(struct Tree* tree, struct Tree* diff, FILE* tex, 
                                                           LOG_PARAMS) {
    diff_log_report();

    int vars_number = get_vars_number();

    struct Node* diff_dest = diff->root;
    struct Node* sum_node  = NULL;

    for (int counter = 0; counter < vars_number; counter++) {

        char var = get_var_by_number(counter);

        NODE_DIFF(tree->root, diff_dest, var);

        if (counter)
            diff_dest->parent = sum_node;

        tree_draw_graph(diff);

        fprintf(tex, "\n \\textbf{Differentiation by %c}\n", var);
        latex_show_derivative(diff, tex);
        TREE_SIMPLIFY_WITH_TEX(diff, tex);

        if (!counter)    
            sum_node = diff->root;

        if (vars_number > 1 && counter < vars_number - 1)
            COMPLEX_DIFF_ADD_SUM(&diff_dest, &sum_node, diff)
    }

    return 0;
}

//===================================================================

int _diff_execute(struct Tree* tree, struct Tree* diff, const char* tex_name, 
                                                                  LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);
    TREE_PTR_CHECK(diff);

    FILE* tex = tree_latex_start(tree);
    if (!tex)
        return -1;

    tree_latex_original_expr(tree, tex);
    TREE_SIMPLIFY_WITH_TEX(tree, tex);
    tree_latex_original_after_simp(tree, tex);

    char answer = 0;

    if (!get_vars_number()) {

        NODE_INIT_CONSTANT(diff->root, 0);
        tree_latex_add_conspect(diff, tex);
        tree_latex_finish(tree, tex, tex_name);

        return 0;
    }
    
    if (get_vars_number() != 1) {
            
        answer = get_var_diff_by();
        if (answer == -1)
            return -1;
    }
    else 
        answer = get_var_by_number(1);

    if (!answer) 
        DIFF_EXECUTE_ALL(tree, diff, tex)
    else
        DIFF_EXECUTE_SINGLE(tree, diff, tex, answer);

    tree_latex_finish(diff, tex, tex_name);

    return 0;
}

//===================================================================

static int _node_is_constant(struct Node* node, char var, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (!node->left_son && !node->right_son) {

        if (node->data_type == CONSTANT || 
           (node->data_type == VARIABLE && node->data.variable != var))

           return 1;
    }

    int left  = 1;
    int right = 1;

    if (node->left_son) {

        left = node_is_constant(node->left_son, var);
        if (left == -1)
            return -1;
    }

    if (node->right_son) {

        right = node_is_constant(node->right_son, var);
        if (right == -1)
            return -1;
    }

    return (right && left);
}

//===================================================================

int _node_diff_execute(struct Node* orig, struct Node* diff, char var, 
                                                           LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    if (orig->parent == No_parent)
        diff->parent =  No_parent;

    switch(orig->data_type) {

        case CONSTANT: {

            return node_init_constant(diff, 0);
        }

        case VARIABLE: {
            
            if (orig->data.variable == var)
                return node_init_constant(diff, 1);
            else
                return node_init_constant(diff, 0);
        }

        case OPERAND: {

            return diff_operand_node(orig, diff, var);
        }

        default: {

            if (node_is_constant(orig, var))
                return node_init_constant(diff, 0);

            error_report(DIFF_INV_DATA_TYPE);
            return -1;
        }
    }
}

//===================================================================

static int _diff_operand_node(struct Node* orig, struct Node* diff, char var, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    int operand = orig->data.operand;

    switch(operand) {

        case ADD: [[fallthrough]];
        case SUB: 
            return diff_operand_add_or_sub(orig, diff, operand, var);

        case MUL: 
            return diff_operand_mul(orig, diff, var);

        case DIV:
            return diff_operand_div(orig, diff, var);

        case POW:
            return diff_operand_pow(orig, diff, var);

        case SIN:
            return diff_operand_sin(orig, diff, var);

        case COS:
            return diff_operand_cos(orig, diff, var);

        case TG:
            return diff_operand_tg(orig, diff, var);

        case CTG:
            return diff_operand_ctg(orig, diff, var);

        case ASIN: [[fallthrough]];
        case ACOS:
            return diff_operand_arcsin_and_arccos(orig, diff, operand, var);

        case ATG: [[fallthrough]];
        case ACTG:
            return diff_operand_arctg_and_arcctg(orig, diff, operand, var);

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }
}

//===================================================================

static int _diff_operand_add_or_sub(struct Node* orig, struct Node* diff, 
                                          int oper, char var, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, (char)oper);

    ADD_LEFT_AND_RIGHT(diff);

    NODE_DIFF(orig->L, diff->L, var);
    NODE_DIFF(orig->R, diff->R, var);

    return 0;
}

//===================================================================

static int _diff_operand_mul(struct Node* orig, struct Node* diff, 
                                             char var, LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, ADD);

    ADD_OPERAND(diff, LEFT,  MUL);
    ADD_OPERAND(diff, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->L);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_DIFF(orig->L, diff->L->L, var);
    NODE_COPY(orig->R, diff->L->R);

    NODE_DIFF(orig->R, diff->R->R, var);
    NODE_COPY(orig->L, diff->R->L);

    return 0;
}

//===================================================================

static int _diff_operand_div(struct Node* orig, struct Node* diff, 
                                             char var, LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_OPERAND(diff, LEFT,  SUB);
    ADD_OPERAND(diff, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_COPY(orig->R, diff->R->L);
    NODE_INIT_CONSTANT(diff->R->R, 2);

    ADD_OPERAND(diff->L, LEFT,  MUL);
    ADD_OPERAND(diff->L, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->L->L);
    ADD_LEFT_AND_RIGHT(diff->L->R);

    NODE_DIFF(orig->L, diff->L->L->L, var);
    NODE_COPY(orig->R, diff->L->L->R);

    NODE_DIFF(orig->R, diff->L->R->R, var);
    NODE_COPY(orig->L, diff->L->R->L);

    return 0;
}

//===================================================================

static int _diff_operand_pow(struct Node* orig, struct Node* diff, char var,
                                                                 LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    double power_value = orig->R->data.constant;

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, MUL);
    ADD_CONSTANT(diff, LEFT,  power_value);
    ADD_OPERAND (diff, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_COPY(orig->L, diff->R->L);
    NODE_INIT_CONSTANT(diff->R->R, power_value - 1);

    return 0;
}

//===================================================================

static int _diff_operand_sin(struct Node* orig, struct Node* diff, char var, 
                                                                 LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, MUL);
    ADD_LEFT_AND_RIGHT(diff);

    NODE_DIFF(orig->L, diff->R, var);

    NODE_INIT_OPERAND(diff->L, COS);
    ADD_LEFT (diff->L);
    NODE_COPY(orig->L, diff->L->L);
   
    return 0;
}

//===================================================================

static int _diff_operand_cos(struct Node* orig, struct Node* diff, char var, 
                                                                 LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, MUL);
    ADD_CONSTANT(diff, LEFT, -1);
    ADD_OPERAND(diff, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_DIFF(orig->L, diff->R->L, var);

    NODE_INIT_OPERAND( diff->R->R, SIN);
    ADD_LEFT( diff->R->R);
    NODE_COPY(orig->L, diff->R->R->L);

    return 0;
} 

//===================================================================

static int _diff_operand_tg(struct Node* orig, struct Node* diff, char var, 
                                                                LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_CONSTANT(diff, LEFT,  1);
    ADD_OPERAND (diff, RIGHT, POW);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_INIT_OPERAND(diff->R->L, COS);
    ADD_LEFT(diff->R->L);
    NODE_COPY(orig->L, diff->R->L->L);

    NODE_INIT_CONSTANT(diff->R->R, 2);

    return 0;
}

//===================================================================

static int _diff_operand_ctg(struct Node* orig, struct Node* diff, char var, 
                                                                 LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_CONSTANT(diff, LEFT,  -1);
    ADD_OPERAND (diff, RIGHT, POW);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_INIT_OPERAND(diff->R->L, SIN);
    ADD_LEFT(diff->R->L);
    NODE_COPY(orig->L, diff->R->L->L);

    NODE_INIT_CONSTANT(diff->R->R, 2);

    return 0;
}

//===================================================================

static int _diff_operand_arcsin_and_arccos(struct Node* orig, struct Node* diff, 
                                                 int oper, char var, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, DIV);

    int sign = 0;
    if (oper == ASIN)
        sign = 1;
    else 
        sign = -1;

    ADD_CONSTANT(diff, LEFT, sign);
    ADD_OPERAND (diff, RIGHT, POW);

    ADD_OPERAND (diff->R, LEFT,  SUB);
    ADD_CONSTANT(diff->R, RIGHT, 0.5);

    ADD_CONSTANT(diff->R->L, LEFT,  1);
    ADD_OPERAND (diff->R->L, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R->L->R);
    NODE_INIT_CONSTANT(diff->R->L->R->R, 2);
    NODE_COPY(orig->L, diff->R->L->R->L);

    return 0;
}

//===================================================================

static int _diff_operand_arctg_and_arcctg(struct Node* orig, struct Node* diff, 
                                                int oper, char var, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff, var);

    NODE_INIT_OPERAND(diff, DIV);

    int sign = 0;
    if (oper == ATG)
        sign = 1;
    else
        sign = -1;

    ADD_CONSTANT(diff, LEFT,  sign);
    ADD_OPERAND (diff, RIGHT, ADD);

    ADD_CONSTANT(diff->R, LEFT,  1);
    ADD_OPERAND (diff->R, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R->R);
    NODE_INIT_CONSTANT(diff->R->R->R, 2);
    NODE_COPY(orig->L, diff->R->R->L);

    return 0;
}

//===================================================================

#undef L
#undef R

//===================================================================

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return tree_save_to_file(diff, filename);
}

//===================================================================

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return node_save_to_file(diff->root, stdout);
}

//===================================================================

int diff_read_cmnd_line(int argc, const char* argv[], const char** logs  , 
                                                      const char** input , 
                                                      const char** output, 
                                                      const char** latex) {

    for (int counter = 1; counter < argc; counter++) {

        #ifdef DIFF_LOGS

            if (!strcmp("-L", argv[counter])) {

                *logs = argv[counter + 1];
                counter += 2;
            }

        #endif

        #ifdef DIFF_READ_FROM_FILE

            if (!strcmp("-I", argv[counter])) {

                *input = argv[counter + 1];
                counter += 2;
            }
            
        #endif

        #ifdef DIFF_OUT_TO_FILE

            if (!strcmp("-O", argv[counter])) {

                *output = argv[counter + 1];
                counter += 2;
            }
            
        #endif

        #ifdef DIFF_LATEX

            if (!strcmp("-T", argv[counter])) {

                *latex = argv[counter + 1];
                counter += 2;
            }
            
        #endif
    } 

    return 0;                                                      
}

