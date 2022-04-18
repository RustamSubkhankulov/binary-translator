#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "../general/general.h"

//===================================================================

static int Graph_counter = 0;

static int Node_counter = 0;

//===================================================================

static const char* Identificator_color = "#a5b8ef";
static const char* Identificator_shape = "diamond";

static const char* Constant_color      = "#e9a0a4";
static const char* Constant_shape      = "ellipse";

static const char* Bin_operand_color    = "#d2c1e6";
static const char* Bin_operand_shape    = "doubleoctagon";

static const char* Unar_operand_color   = "#a8def0";
static const char* Unar_operand_shape   = "octagon";

static const char* Key_node_color      = "#f5b342";
static const char* Key_node_shape      = "pentagon";

static const char* Std_func_color      = "#32a889";
static const char* Std_func_shape      = "rectangle";

static const char* Left_edge_color      = "#3b4bde";
static const char* Right_edge_color     = "#b30000";

//===================================================================

int _print_node_data_type(struct Node* node, FILE* output, LOG_PARAMS) {

    //tree_log_report();
    NODE_PTR_CHECK(node);

    if (output == NULL) 
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) {

        case CONSTANT: 
        {
            fprintf(output, " constant ");
            break;
        }

        case IDENTIFICATOR: 
        {
            fprintf(output, " identificator ");
            break;
        }

        case BIN_OPERAND: 
        {
            fprintf(output, " binary operand ");
            break;
        }

        case UNAR_OPERAND:
        {
            fprintf(output, " unary operand ");
            break;
        }

        case KEY_NODE:
        {
            fprintf(output, " key node ");
            break;
        }

        case STD_FUNC_NODE:
        {
            fprintf(output, " standart function");
            break;
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

int _print_std_func(int std_func_code, FILE* output, LOG_PARAMS)
{
    if (!output)
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    for (int counter = 0; counter < Std_functions_number; counter++)
    {
        if (std_func_code == Std_functions[counter].code)
        {
            fprintf(output, " %s ", Std_functions[counter].meaning);
            return 0;
        }
    }

    error_report(INV_STD_FUNC_CODE);
    return -1;
}
//===================================================================

int _print_key_node_data(int key_node_code, FILE* output, LOG_PARAMS) {

    if (!output)
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    for (int counter = 0; counter < Key_node_number; counter++)
    {
        if (key_node_code == Key_node_info_table[counter].key_node_code)
        {
            fprintf(output, " %s ", Key_node_info_table[counter].name);
            return 0;
        }
    }

    error_report(INV_KEY_NODE_CODE);
    return -1;
}

//===================================================================

int _print_operand(int oper_code, FILE* output, LOG_PARAMS) {

    if (output == NULL) 
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    for (int counter = 0; counter < Calc_functions_number;  counter++) 
    {
        if (oper_code == Calc_functions[counter].code) 
        {
            fprintf(output, "%s", Calc_functions[counter].name);
            return 0;
        }
    }

    for (int counter = 0; counter < Std_cmp_signs_number; counter++)
    {
        if (oper_code == Std_cmp_signs[counter].code) 
        {
            fprintf(output, " %s ", Std_cmp_signs[counter].name);
            return 0;
        }
    }

    if (IS_OPERAND(oper_code))
    {
        fprintf(output, " %c ", oper_code);
        return 0;
    }

    error_report(INV_OPER_CODE);
    return -1;
}

//===================================================================

int _print_node_data(struct Node* node, FILE* output, LOG_PARAMS) {

    //tree_log_report();
    NODE_PTR_CHECK(node);

    if (output == NULL) 
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) 
    {
        case CONSTANT: 
        {
            fprintf(output, "%g", node->data.constant);
            break;
        }

        case IDENTIFICATOR: 
        {
            fprintf(output, "%ld", node->data.id_hash);
            break;
        }

        case KEY_NODE:
        {
            return print_key_node_data(node->data.key_node_code, output);
        }

        case BIN_OPERAND : [[fallthrough]];
        case UNAR_OPERAND:
        {
            return print_operand(node->data.operand_code, output);
        }

        case STD_FUNC_NODE:
        {
            return print_std_func(node->data.std_func_code, output);
        }

        default: 
        {
            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

int _tree_dump(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    fprintf(logs_file, "<div style = \"background-color: "
                       "lightgrey; font-size: 18 \"><pre>");

    fprintf(logs_file, "\n\n\n<p style = \" text-align: center; \">"
                                     "<b> Tree structure </b></p>\n");

    fprintf(logs_file, "\n Address: <%p>\n", tree);

    fprintf(logs_file, "\n ROOT ELEMENT: <%p> Data: <", tree->root);
    print_node_data(tree->root, logs_file);

    fprintf(logs_file, "> Data type: <");
    print_node_data_type(tree->root, logs_file);
    fprintf(logs_file, ">\n");

    fprintf(logs_file, "\n</pre></div>");   

    int ret = tree_print(tree);
    if (ret == -1)
        return -1;

    #ifdef TREE_GRAPHVIZ

        ret = tree_draw_graph(tree);
        if (ret == -1)
            return -1;

    #endif

    return 0; 
}

//===================================================================

int _tree_save_to_file(struct Tree* tree, const char* filename, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    FILE* tree_output = open_file(filename, "wb");
    if (tree_output == NULL)
        return 0;

    int ret = node_save_to_file(tree->root, tree_output);
    if (ret == -1)
        return -1;

    fprintf(tree_output, "\n");

    return close_file(tree_output);
}

//===================================================================

int _node_save_to_file(struct Node* node, FILE* output, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son == NULL && node->right_son == NULL) {

        fprintf(output, " ( ");
        print_node_data(node, output);
        fprintf(output, " ) ");
    }

    else {

        fprintf(output, " ( ");

        print_node_data(node, output);

        if (node->left_son) {

            int ret = node_save_to_file(node->left_son, output);
            if (ret == -1)
                return -1;
        }

        if (node->right_son) {

            int ret = node_save_to_file(node->right_son, output);
            if (ret == -1)
                return -1;
        }

        fprintf(output, " ) ");
    }

    return 0;
}

//===================================================================

int _tree_print(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return node_visiter(tree->root, _node_print);

    return 0;
}

//===================================================================

int _node_dot_fill(struct Node_dot* node_dot, FILE* graph_output, 
                                          int father, LOG_PARAMS) {

    tree_log_report();
    NODE_DOT_PTR_CHECK(node_dot);

    node_dot->graph   = graph_output;
    node_dot->father  = father;

    return 0;
}

//===================================================================

int _tree_draw_legend(FILE* graph, LOG_PARAMS) {

    tree_log_report();
    
    if (!graph)
    {
        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(graph, "\n subgraph clusterlegend{\nfontsize = 40; rankdir = LR; style = filled; color = lightgrey;  label = \" Legend\"\n;\n rank = same; KEY_NODE; UNARY; BINARY; ID; CONST; STD_FUNC;}\n");

    fprintf(graph, "\n { rankdir = TB; node[fontsize = 40];\n");

    fprintf(graph, "\n KEY_NODE[ label = \"KEY NODE\", color = \"%s\", shape = \"%s\"];\n", Key_node_color, 
                                                                                            Key_node_shape);

    fprintf(graph, "\n UNARY[ label = \"UNARY OPERAND\", color = \"%s\", shape = \"%s\"];\n", Unar_operand_color, 
                                                                                              Unar_operand_shape);

    fprintf(graph, "\n BINARY[ label = \"BINARY OPERAND\", color = \"%s\", shape = \"%s\"];\n", Bin_operand_color, 
                                                                                                Bin_operand_shape);

    fprintf(graph, "\n ID[ label = \"IDENTIFICATOR\", color = \"%s\", shape = \"%s\"];\n", Identificator_color, 
                                                                                           Identificator_shape);

    fprintf(graph, "\n CONST[ label = \"CONSTANT\", color = \"%s\", shape = \"%s\"];\n", Constant_color, 
                                                                                         Constant_shape);

    fprintf(graph, "\n STD_FUNC[ label = \"STD FUNC\", color = \"%s\", shape = \"%s\"];\n", Std_func_color,
                                                                                            Std_func_shape);

    fprintf(graph, "\n KEY_NODE -> UNARY   [color = lightgrey];\n");
    fprintf(graph, "\n UNARY    -> BINARY  [color = lightgrey];\n");
    fprintf(graph, "\n BINARY   -> ID      [color = lightgrey];\n");
    fprintf(graph, "\n ID       -> CONST   [color = lightgrey];\n");
    fprintf(graph, "\n CONST    -> STD_FUNC[color = lightgrey];\n}\n");

    return 0;
}

//===================================================================

// static int add_statement_node_dot(Node* statement, LOG_PARAMS)
// {
//     if (!Statement_nodes_capacity)
//     {
//         Statement_nodes_dot = (Node**)calloc(sizeof(Node*), 5);
//         Statement_nodes_capacity = 5;

//         if (!Statement_nodes_dot)
//         {
//             return -1;
//         }
//     }
    
// 

//-------------------------------------------------------------------

// static int print_statement_ranksame(FILE* graph, LOG_PARAMS)
// {
//     if (!graph)
//     {
//         return -1;
//     }

//     fprintf(graph, "\n subgraph clusterstatements{ rankdir = TB; rank = same; \n");

//     for (int counter = 0; counter < Statement_nodes_counter; counter++) 
//     {
//         fprintf(graph, " NODE%p; ", Statement_nodes_dot[counter]);
//     }  

//     fprintf(graph, "}\n");
//     return 0;
// }

//===================================================================

int _tree_draw_graph(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    FILE* graph = fopen(TEMP_DIR "diff_tree_graph.txt", "wb");

    if (graph == NULL) {

        error_report(FOPEN_ERROR);
        return -1;
    }

    fprintf(graph, "digraph G{\n");
    //fprintf(graph, "rankdir=LR;\n");
    fprintf(graph, "node[ style = filled, fontsize = 24];\n");
    fprintf(graph, "edge[ arrowhead = vee, fontsize = 24 ];\n");
    //fprintf(graph, "splines = false;\n");

    int ret = tree_draw_legend(graph);
    if (ret == -1)
        return -1;

    
    //
    //print_statement_ranksame(graph, LOG_ARGS);
    //

    fprintf(graph, "subgraph clusternodes{\n rankdir = TB; color = white;\n");

    struct Node_dot node_dot = { 0 };
    node_dot_fill(&node_dot, graph, 0);
    node_dot.root = tree->root;

    node_draw_graph(tree->root, &node_dot);

    fprintf(graph, "\n}\n}\n");
    fclose(graph);

    ret = graph_call_dot();
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _graph_call_dot(LOG_PARAMS) {

    tree_log_report();

    char command_buf[System_cmnd_buf_size] = { 0 };
    sprintf(command_buf, "dot " TEMP_DIR "diff_tree_graph.txt -Tpng"
                         " -o " TEMP_DIR "diff_images/diff_tree_graph%d.png",
                                                              Graph_counter);

    int ret = system(command_buf);
    if (ret != 0)
        return -1;

    fprintf(logs_file, "\n <img width = 100%% src =" TEMP_DIR "diff_images/diff_tree_graph%d.png"
                                                         " alt = \"Tree graph has not found\">\n",
                                                                                   Graph_counter);

    Graph_counter++;

    return 0;
}

//===================================================================

static int _get_node_dot_parameters(struct Node* node, const char** node_color, 
                                                       const char** node_shape, LOG_PARAMS) {

    switch (node->data_type) 
    {
        case CONSTANT: 
        {
            *node_color = Constant_color;
            *node_shape = Constant_shape;
            break;
        }

        case IDENTIFICATOR: 
        {
            *node_color = Identificator_color;
            *node_shape = Identificator_shape;
            break;
        }

        case BIN_OPERAND: 
        {
            *node_color = Bin_operand_color;
            *node_shape = Bin_operand_shape;
            break;
        }

        case UNAR_OPERAND:
        {
            *node_color = Unar_operand_color;
            *node_shape = Unar_operand_shape;
            break;
        }

        case KEY_NODE:
        {
            *node_color = Key_node_color;
            *node_shape = Key_node_shape;
            break;
        }

        case STD_FUNC_NODE:
        {
            *node_color = Std_func_color;
            *node_shape = Std_func_shape;
            break;
        }

        default: 
        {
            error_report(INV_NODE_DATA_TYPE);
            return -1;
        }
    }

    return 0;
}

//===================================================================

int _node_draw_graph(struct Node* node, struct Node_dot* node_dot, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    int node_number = Node_counter;
    Node_counter++;

    //
    //if (node->data_type == KEY_NODE && node->data.key_node_code == STATEMENT_ND)
    //    add_statement_node_dot(node, LOG_ARGS);
    //

    if (node->left_son)
    {
        fprintf(node_dot->graph, "\n NODE%p -> NODE%p[label = \"L\", color = \"%s\"];\n", node, node->left_son, Left_edge_color);
    }

    if (node->right_son)
    {
        fprintf(node_dot->graph, "\n NODE%p -> NODE%p[label = \"R\", color = \"%s\"];\n", node, node->right_son, Right_edge_color);
    }

    if (node->parent != NULL && node->parent != No_parent)
    {
        fprintf(node_dot->graph, "\n NODE%p -> NODE%p[style = dotted, label = \"P\"];\n", node, node->parent);
    }

    const char* node_color = 0;
    const char* node_shape = 0;

    int ret = get_node_dot_parameters(node, &node_color, &node_shape);
    if (ret == -1) 
    {
        error_report(DIFF_INV_DATA_TYPE);
        return -1;
    }   

    fprintf(node_dot->graph, "\n NODE%p [label = \"", node);
    if (node_dot->root == node)
    {
        fprintf(node_dot->graph, " ROOT \n");
    }
    fprintf(node_dot->graph, "<%p>\n p: <%p>\n L: <%p>\n R: <%p>\n\n", node, 
                                                                        node->parent, 
                                                                        node->left_son, 
                                                                        node->right_son);
    print_node_data(node, node_dot->graph);
    fprintf(node_dot->graph, "\" shape = \"%s\",",          node_shape);
    fprintf(node_dot->graph, "   color = \"%s\"]; \n", node_color);

    if (node->left_son != NULL) 
    {
        node_dot_fill(node_dot, node_dot->graph, node_number);
        
        ret = node_draw_graph(node->left_son, node_dot);
        if (ret == -1)
            return -1;
    }

    if (node->right_son != NULL) 
    {
        node_dot_fill(node_dot, node_dot->graph, node_number);
        
        ret = node_draw_graph(node->right_son, node_dot);
        if (ret == -1)
            return -1;
    }

    return 0;
}

//===================================================================

int _tree_clear_check(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return is_memory_clear(tree, 1, sizeof(tree));
}

//===================================================================

int _tree_cleaning(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return clear_memory(tree, 1, sizeof(struct Tree));
}

//===================================================================

int _tree_poisoning(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    unsigned char* base = (unsigned char*)tree;

    for (int counter = 0; 
             counter < (int)sizeof(struct Tree); 
             counter++) {

        base[counter] = Poison_value;
    }

    return 0;
}

//===================================================================

int _tree_poison_check(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    unsigned char* base = (unsigned char*)tree;

    for (int counter = 0; 
             counter < (int)sizeof(struct Tree); 
             counter++) {

        if (base[counter] != Poison_value)
            return 0;
    }

    return 1;
}

//===================================================================

int _node_add_right_son(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->right_son != NULL) 
    {
        error_report(ADD_SON_ERROR);
        return -1;
    }

    void* node_ptr = node_allocate_memory();

    if (node_ptr == NULL)
        return -1;

    node->right_son = (Node*)node_ptr;
    node->right_son->parent = node;

    return 0;
}

//===================================================================

int _node_add_left_son(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) 
    {
        error_report(ADD_SON_ERROR);
        return -1;
    }

    void* node_ptr = node_allocate_memory();

    if (node_ptr == NULL)
        return -1;

    node->left_son = (Node*)node_ptr;
    node->left_son->parent = node;

    return 0;
}

//===================================================================

int _node_add_sons(struct Node* node, LOG_PARAMS) {

    tree_log_report();

    int ret = node_add_left_son(node);
    if (ret == -1)
        return -1;

    ret = node_add_right_son(node);
    if (ret == -1) 
        return -1;

    return 0;
}

//===================================================================

int _node_init_constant(struct Node* node, double value, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type = CONSTANT;
    node->data.constant = value;

    return 0;
}

//===================================================================

int _node_init_identificator(struct Node* node, int64_t id_hash, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type    = IDENTIFICATOR;
    node->data.id_hash = id_hash;

    return 0;
}

//===================================================================

int _node_init_key_node(struct Node* node, int key_node_code, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type          = KEY_NODE;
    node->data.key_node_code = key_node_code;

    return 0;
}

//===================================================================

int _node_init_bin_operand(struct Node* node, int oper, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type          = BIN_OPERAND;
    node->data.key_node_code = oper;

    return 0;
}

//===================================================================

int _node_init_unar_operand(struct Node* node, int oper, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type          = UNAR_OPERAND;
    node->data.key_node_code = oper;

    return 0;
}

//===================================================================

int _node_init_std_func_node(struct Node* node, int code, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type          = STD_FUNC_NODE;
    node->data.std_func_code = code;

    return 0;
}

//===================================================================

int _tree_delete_branch(struct Node** node_ptr, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node_ptr);

    int ret = node_delete_branch(*node_ptr);
    if (ret == -1)
        return -1;

    *node_ptr = NULL;

    return 0;
}

//===================================================================

int _node_delete_branch(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) 
    {
        int ret = node_delete_branch(node->left_son);
        if (ret ==  -1)
            return -1;
    }

    if (node->right_son != NULL) 
    {
        int ret = node_delete_branch(node->right_son);
        if (ret == -1)
            return -1;        
    }

    if (node == NULL) 
    {
        error_report(INV_NODE_PTR);
        return -1;
    }

    free(node);

    return 0;
}

//===================================================================

void* _node_allocate_memory(LOG_PARAMS) {

    tree_log_report();

    void* memory = calloc(1, sizeof(struct Node));

    if (memory == NULL) 
        error_report(CANNOT_ALLOCATE_MEM);

    return memory;
}

//===================================================================

int _tree_ctor(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    int is_poison = tree_poison_check(tree);
    int is_clear  = tree_clear_check(tree);

    if (is_poison == 0 && is_clear == 0) 
    {
        error_report(TREE_STRUCT_CTOR_UNPREPARED);
        return -1;
    }

    if (is_poison)
        tree_cleaning(tree);

    return 0;
}

//===================================================================

int _tree_dtor(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    TREE_VERIFICATION(tree);

    int ret = tree_free_memory(tree);
    if (ret == -1)
        return -1;

    tree_poisoning(tree);

    return 0;
}

//===================================================================

int _tree_free_memory(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return node_visiter(tree->root, _node_destruct);
}

//===================================================================

int _node_visiter(struct Node* node, int (*node_function)(struct Node* node, LOG_PARAMS), 
                                                                             LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) 
    {    
        if (node_visiter(node->left_son, node_function) == -1)
            return -1;
    }

    if (node->right_son != NULL) 
    {    
        if (node_visiter(node->right_son, node_function) == -1)
            return -1;
    }

    return node_function(node, LOG_ARGS);
}

//===================================================================

int _tree_validator(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    if (node_visiter(tree->root, _node_validator) == -1 )
        return -1;

    else  
    {
        #ifdef TREE_DEBUG

            tree_dump(tree);

        #endif

        return 0;
    }
}

//===================================================================

// int is_function_operand(struct Node* node) {

//     if (!node)
//         return 0;

//     for (int counter = 0;
//              counter < Functions_number;
//              counter++) 

//         if (node->data.operand == Functions[counter].code)
//             return 1;

//     return 0;
// }

//===================================================================

// int symb_is_var_name(char symb) {

//     return (symb < 'a' || symb > 'z')? 0: 1;
// }

//===================================================================

// int symb_is_operand(int symb) {

//     for (int counter = 0; 
//              counter < Operands_number; 
//              counter++) 

//         if (symb == Operands[counter])
//             return 1;

//     return 0;
// }

//===================================================================

int _node_validator(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    fflush(logs_file);
    NODE_PTR_CHECK(node);

    int err_val  = 0;

    // if (node->data_type == OPERAND 
    // && (node->left_son == NULL && node->right_son == NULL)) 
    // {
    //     error_report(OPER_NO_SONS);
    //     err_val++;
    // }

    // if (node->data_type == OPERAND
    // && is_function_operand(node)
    // && node->left_son == NULL) 
    // {
    //     error_report(INV_SONS_FUNCTION_OPERAND);
    //     err_val++;
    // }

    // if ((node->data_type == CONSTANT || node->data_type == VARIABLE) 
    //  && (node->right_son != NULL     || node->left_son  != NULL)) 
    //  {
    //     error_report(NODE_NO_SONS);
    //     err_val++;
    // }

    // if (node->data_type == VARIABLE && !symb_is_var_name(node->data.variable)) 
    // {
    //     error_report(INV_VAR_NAME);
    //     err_val++;
    // }

    // if (node->right_son != NULL 
    //  && node->left_son  != NULL 
    //  && node->left_son == node->right_son) 
    //  {
    //     error_report(NODE_EQUAL_SONS);
    //     err_val++;
    // }

    // if (node->parent   != No_parent
    // && node->parent    != NULL
    // && node->left_son  != NULL
    // && node->right_son != NULL) 
    // {
    //     if (node->parent->left_son  != node 
    //      && node->parent->right_son != node) 
    //     {
    //         error_report(NODE_INV_PARENT);
    //         err_val++;
    //     }
    // }

    return (err_val)? -1: 0; 
}

//===================================================================

int _node_print(struct Node* node, LOG_PARAMS) {

    //tree_log_report();
    NODE_PTR_CHECK(node);

    fprintf(logs_file, "<div style = \"background-color: lightgrey; font-size: 18\"><pre>");

    fprintf(logs_file, "\n<b>Node: <%p> </b> Data: <", node);
    int err_val = print_node_data(node, logs_file);  $

    fprintf(logs_file, "> Data type: <");
    err_val = print_node_data_type(node, logs_file); $

    fprintf(logs_file, ">\n ""Left son: <%p> Right son: <%p> Parent: <%p>\n",
                                                             node-> left_son, 
                                                             node->right_son,
                                                               node->parent);
    fprintf(logs_file, "</pre></div>");

    return 0;
}

//===================================================================

int _node_destruct(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    free(node);

    return 0;
}

//===================================================================

int _nodes_connect_right(Node* parent, Node* son, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(parent);
    NODE_PTR_CHECK(son);

    son->parent = parent;
    parent->right_son = son;

    return 0;
}

//-------------------------------------------------------------------

int _nodes_connect_left (Node* parent, Node* son, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(parent);
    NODE_PTR_CHECK(son);

    son->parent = parent;
    parent->left_son = son;

    return 0;
}