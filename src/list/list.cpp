#include <stdlib.h>
#include <string.h>

//===================================================================

#include "list.h"
#include "../general/general.h"
#include "../bintrans/optimizer.h"

//===================================================================

//Static variables used to ensure that pointers to allocated 
//dynamic memory have not changed

#ifdef LIST_PTR_CNTRL

    static void* List_data_ptr = NULL;

    static void* List_next_ptr = NULL;

    static void* List_prev_ptr = NULL;

#endif 

//===================================================================

//Counter for graphes, drawn by GraphViz (for logs)
static int Graph_counter = 0;

//===================================================================

#ifdef LIST_HASH

    static int _list_save_hash(struct List* list FOR_LOGS(, LOG_PARAMS));

    static int _list_hash_check(struct List* list FOR_LOGS(, LOG_PARAMS));

#endif

//===================================================================

static int _list_set_zero(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_poisoning(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_clear_check(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_free_memory(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_poison_check(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_allocate_memory(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_check_connections(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_set_next_in_order(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_out(struct List* list, FILE* output FOR_LOGS(, LOG_PARAMS));

static int _list_pop_check(struct List* list, unsigned int index
                                         FOR_LOGS(, LOG_PARAMS));
                                                      
static int _list_push_check(struct List* list, unsigned int index  
                                           FOR_LOGS(,LOG_PARAMS));

static int _list_check_free_elements(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_set_prev_to_minus_one(struct List* list FOR_LOGS(, LOG_PARAMS));

static int _list_prepare_after_increase(struct List* list, size_t prev_capacity 
                                                        FOR_LOGS(, LOG_PARAMS));

#ifdef LIST_PTR_CNTRL

    static int _list_pointers_values_check(struct List* list FOR_LOGS(, LOG_PARAMS));

    static int _update_list_pointers_values(struct List* list FOR_LOGS(, LOG_PARAMS));

#endif 

//===================================================================

#ifdef LIST_GRAPHVIZ

int _list_draw_graph(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    FILE* graph = fopen(LOG_DIR "list_graph.txt", "wb");

    //printf("\n " LOG_DIR "list_graph.txt" "\n");

    fprintf(graph, "digraph G{\n");
    fprintf(graph, "rankdir=HR;\n");
    fprintf(graph, "{rank = same; \n");

    for (unsigned int counter = 0; counter < list->capacity; counter++) {

        fprintf(graph, "ELEMENT%u [shape=none, margin=0, style=rounded,"
                                                " label=<\n", counter);

        fprintf(graph, "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\""
                                                      " CELLPADDING=\"4\">\n"); 
        if (list->prev[counter] == -1)

            fprintf(graph, "<TR><TD COLSPAN=\"3\" BGCOLOR=\"#c2e7c2\"> INDEX = %u"
                                                      " </TD></TR>\n", counter);
        else if (counter == list->head)

            fprintf(graph, "<TR><TD COLSPAN=\"3\" BGCOLOR=\"#e7c2c2\"> INDEX = %u"
                                                      " </TD></TR>\n", counter);
        else if (counter == list->tail)

            fprintf(graph, "<TR><TD COLSPAN=\"3\" BGCOLOR=\"#c2cce7\"> INDEX = %u"
                                                      " </TD></TR>\n", counter);
        else

            fprintf(graph, "<TR><TD COLSPAN=\"3\" BGCOLOR=\"lightgrey\"> INDEX = %u"
                                                      " </TD></TR>\n", counter);

        if (counter == list->tail)
            fprintf(graph, "<TR><TD COLSPAN=\"3\">  <b> TAIL OF LIST"
                                                 "</b></TD></TR>\n");

        if (counter == list->head)
            fprintf(graph, "<TR><TD COLSPAN=\"3\">  <b> HEAD OF LIST"
                                                 "</b></TD></TR>\n");

        if (list->prev[counter] == -1)
            fprintf(graph, "<TR><TD COLSPAN=\"3\"> <b> FREE ELEMENT "
                                                 "</b></TD></TR>\n"); 

        fprintf(graph, " <TR><TD PORT=\"prev\"> PREV = %d</TD>\n",
                                            list->prev[counter]);

        fprintf(graph, " <TD PORT=\"data\"> DATA =  ");

        if (list->data[counter] != NULL)

        #ifndef ADD_INSTR_NAME

            fprintf(graph, "%x ", list->data[counter]->oper_code);

        #else

            fprintf(graph, "%s ", list->data[counter]->name_str);

        #endif 

        else fprintf(graph, "%p", list->data[counter]);

        fprintf(graph, " </TD>\n");

        fprintf(graph, "<TD PORT=\"next\"> NEXT = %d</TD></TR></TABLE>>];\n", 
                                                        list->next[counter]);

        if (counter != list->capacity -  1)
            fprintf(graph, "ELEMENT%u -> ELEMENT%u [color = \"white\"];\n", counter, 
                                                                            counter + 1);
        
        if (list->next[counter] != 0 && list->prev[counter] != -1)
            fprintf(graph, "ELEMENT%u:<next> -> ELEMENT%d:<prev>;\n", counter, 
                                                         list->next[counter]);
    }

    fprintf(graph, "}\n}\n");
    fclose(graph);

    char command_buffer[SYSTEM_COMMAND_BUF_SIZE] = { 0 };
    sprintf(command_buffer, "dot " LOG_DIR "list_graph.txt -Tpng"
                            " -o " LOG_DIR "list_images/list_graph%d.png", 
                                                            Graph_counter);

    int ret = system(command_buffer);
    if (ret != 0)
        return -1;

    fprintf(logs_file, "\n <img src = list_images/list_graph%d.png "
                                     "alt = \"List graph has not found\">\n\n", 
                                                                Graph_counter);

    Graph_counter++;

    return 0;
}

//===================================================================

int _list_draw_graph_logical(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->size == 0)
        return 0;

    FILE* graph = fopen(LOG_DIR "list_graph.txt", "wb");

    fprintf(graph, "digraph G{\n");
    fprintf(graph, "rankdir=HR;\n");
    fprintf(graph, "{rank = same; \n");

    unsigned int index = list->head;

    do {

        fprintf(graph, "ELEMENT%u [shape=none, margin=0, style=rounded,"
                                                " label=<\n", index);

        fprintf(graph, "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\""
                                                      " CELLPADDING=\"4\">\n"); 

        fprintf(graph, "<TR><TD COLSPAN=\"3\" BGCOLOR=\"lightgrey\"> INDEX = %u"
                                                      " </TD></TR>\n", index);

        if (index == list->tail)
            fprintf(graph, "<TR><TD COLSPAN=\"3\">  <b> TAIL OF LIST"
                                                 "</b></TD></TR>\n");

        if (index == list->head)
            fprintf(graph, "<TR><TD COLSPAN=\"3\">  <b> HEAD OF LIST"
                                                 "</b></TD></TR>\n");

        fprintf(graph, " <TR><TD PORT=\"prev\"> PREV = %d</TD>\n",
                                              list->prev[index]);

        fprintf(graph, " <TD PORT=\"data\"> DATA =  ");

        if (list->data[index] != NULL)

        #ifndef ADD_INSTR_NAME

            fprintf(graph, "%x ", list->data[index]->oper_code);

        #else

            fprintf(graph, "%s ", list->data[index]->name_str);

        #endif
            
        else fprintf(graph, "%p", list->data[index]);

        fprintf(graph, " </TD>\n");

        fprintf(graph, " <TD PORT=\"next\"> NEXT = %d</TD></TR></TABLE>>];\n", 
                                                         list->next[index]);

        if (index != list->tail)
            fprintf(graph, "ELEMENT%u:<next> -> ELEMENT%d:<prev>;\n", index, list->next[index]);

        index = (unsigned)list->next[index];

    } while (index != 0);

    fprintf(graph, "}\n}\n");
    fclose(graph);

    char command_buffer[SYSTEM_COMMAND_BUF_SIZE] = { 0 };
    sprintf(command_buffer, "dot " LOG_DIR "list_graph.txt -Tpng"
                            " -o " LOG_DIR "list_images/list_graph%d.png", 
                                                            Graph_counter);

    int ret = system(command_buffer);
    if (ret != 0)
        return -1;

    fprintf(logs_file, "\n <img src = list_images/list_graph%d.png "
                                     "alt = \"List graph has not found\">\n\n", 
                                                                Graph_counter);

    Graph_counter++;

    return 0;
}

#endif 

//===================================================================

#ifdef LIST_HASH

static int _list_save_hash(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    list->base_hash = get_hash(list, sizeof(List) 
                                 - 4 * sizeof(int64_t));

    if (list->data == NULL) {

        error_report(LIST_DATA_IS_NULL);
        return -1;
    }

    if (list->next == NULL) {

        error_report(LIST_NEXT_IS_NULL);
        return -1;
    }

    if (list->prev == NULL) {

        error_report(LIST_PREV_IS_NULL);
        return -1;
    }

    list->data_hash =
           get_hash(list->data, sizeof(list_elem_t) * list->capacity);

    list->next_hash = 
           get_hash(list->next, sizeof(int) * list->capacity);

    list->prev_hash = 
           get_hash(list->prev, sizeof(int) * list->capacity);

    return 0;
}

#endif

//===================================================================

#ifdef LIST_HASH

static int _list_hash_check(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    int64_t base_hash = get_hash(list, sizeof(List) 
                                 - 4 * sizeof(int64_t));

    int64_t data_hash =
             get_hash(list->data, sizeof(list_elem_t) * list->capacity);

    int64_t next_hash = 
             get_hash(list->next, sizeof(int) * list->capacity);

    int64_t prev_hash =
             get_hash(list->prev, sizeof(int) * list->capacity);

    if (list->base_hash != base_hash) {

        error_report(LIST_INV_BASE_HASH);
        return 0;
    }
    
    if (list->data_hash != data_hash) {

        error_report(LIST_INV_DATA_HASH);
        return 0;
    }
    
    if (list->next_hash != next_hash) {

        error_report(LIST_INV_NEXT_HASH);
        return 0;
    }

    if (list->prev_hash != prev_hash) {

        error_report(LIST_INV_PREV_HASH);
        return 0;
    }

    return 1;
}

#endif

//===================================================================

static int _list_recalloc_buffers(struct List* list, size_t prev_capacity 
                                                  FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    void* new_data_ptr = my_recalloc(list->data, list->capacity, 
                                     prev_capacity, sizeof(list_elem_t));
    if (new_data_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    else 
        list->data = (list_elem_t*)new_data_ptr;


    void* new_next_ptr = my_recalloc(list->next, list->capacity,
                                     prev_capacity, sizeof(int));
    if (new_next_ptr == NULL) {
        
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    else 
        list->next = (int*)new_next_ptr;


    void* new_prev_ptr = my_recalloc(list->prev, list->capacity, 
                                     prev_capacity, sizeof(int));
    if (new_prev_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    else
        list->prev = (int*)new_prev_ptr;

    return 0;
}

//===================================================================

static int _list_decrease(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    size_t prev_capacity = list->capacity;
    list->capacity /= 2;
    
    if (list->capacity < List_start_capacity)
        list->capacity = List_start_capacity;

    int ret = list_recalloc_buffers(list, prev_capacity);
    if (ret == -1)
        return -1;

    #ifdef LIST_PTR_CNTRL

        ret = update_list_pointers_values(list);
        if (ret == -1)
            return -1;

    #endif 

    list->next[list->capacity - 1] = 0;

    #ifdef LIST_HASH

        ret = list_save_hash(list);
        if (ret == -1)
            return -1;
    #endif

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    return 0;
}

//===================================================================

static int _list_increase(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->size == List_max_capacity - 1) {

        error_report(LIST_OVERFLOW);
        return -1;
    }

    size_t prev_capacity = list->capacity;
    list->capacity *= 2;
    
    if (list->capacity > List_max_capacity)
        list->capacity = List_max_capacity;

    void* new_data_ptr = my_recalloc(list->data,  list->capacity, 
                                     prev_capacity,  sizeof(list_elem_t));
    if (new_data_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }
    else 
        list->data = (list_elem_t*)new_data_ptr;

    void* new_next_ptr = my_recalloc(list->next, list->capacity,
                                     prev_capacity, sizeof(int));
    if (new_next_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }
    else    
        list->next = (int*)new_next_ptr;

    void* new_prev_ptr = my_recalloc(list->prev, list->capacity,
                                     prev_capacity, sizeof(int));

    if (new_prev_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }
    else
        list->prev = (int*)new_prev_ptr;

    #ifdef LIST_PTR_CNTRL

        update_list_pointers_values(list);

    #endif 

    int ret = list_prepare_after_increase(list, prev_capacity);
    if (ret == -1)
        return -1;

    #ifdef LIST_HASH

        ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    return 0;
}

//===================================================================

static int _list_prepare_after_increase(struct List* list, size_t prev_capacity FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    list->free = list->size + 1;

    for (unsigned counter = (unsigned)prev_capacity; 
                  counter < (unsigned)list->capacity; 
                  counter ++) {

        if (counter != list->capacity - 1)
            list->next[counter] = (int)(counter + 1);
        else 
            list->next[counter] = 0;

        list->prev[counter] = -1;
    }

    return 0;
}

//===================================================================

// int _list_search(struct List* list, list_elem_t elem FOR_LOGS(, LOG_PARAMS))
// {
//     list_log_report();
//     LIST_POINTER_CHECK(list);

//     #ifdef LIST_VALID_CHECK

//         if (list_validator(list) == -1)
//             return -1;

//     #endif 

//     if (list->size == 0)
//         return ELEMENT_NOT_FOUND;
    
//     int cur_index = list->head;

//     for (unsigned int counter = 0;
//                       counter < list->size;
//                       counter ++)
//     {



//         #ifdef STRCMP_COMP

//             //if (strcmp("from", elem) == 0) printf("\n %p \n ", list->data[counter]);

//             if (list->data[cur_index] != NULL)
//             {
                
//                 // if (strcmp("from", elem) == 0)
//                 // {
//                 //     printf("\n comparing %s %s", elem, list->data[cur_index]);
//                 // }
                
//                 //if (strcmp("from", elem) == 0) printf("\n cur index %d \n", cur_index);

//                 //if (strcmp("from", elem) == 0) printf("\n cur data %s elem %s \n", list->data[cur_index], elem);

//                 if (strcmp(elem, list->data[cur_index]) == 0)
//                     return cur_index;
//                 else 
//                     cur_index = list->next[cur_index];
                
                               
//                 // if (strcmp("from", elem) == 0)
//                 // {
//                 //     printf("\n result %d \n ", strcmp(elem, list->data[cur_index]));
//                 // }
//             }
            
//         #else 

//             if (elem == list->data[cur_index])
//                 return cur_index;
//             else 
//                 cur_index = list->next[cur_index];

//         #endif 
//     }

//     return ELEMENT_NOT_FOUND;
// }

//===================================================================

int _list_get_by_logical_number(struct List* list, int number, int* err
                                                FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (number <= 0 || number > (int)list->size) {

        error_report(INV_LOGICAL_NUMBER);
        *err = -1;
        return -1;
    }

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
        {
            *err = -1;
            return -1;
        }

    #endif 

    if (list->is_linearized) 

        return number + (int)list->head - 1;

    else {

        int index = (int)list->head;
        unsigned counter = 1;

        while (counter < (unsigned)number) {

            index = list->next[index];
            counter++;
        }
            
        return index;
    }
}

//===================================================================

int _list_linearize(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    if (list->is_linearized == 1)
        return 0;

    list_elem_t* new_data_ptr = (list_elem_t*)calloc(list->capacity, sizeof(list_elem_t));
    if (new_data_ptr == NULL){

        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    unsigned index = list->head;

    for (unsigned counter = 1; 
                  counter <= list->size; counter++) {

        new_data_ptr[counter] = list->data[index];
        index = (unsigned)list->next[index];
    }

    #ifdef LIST_PTR_CNTRL

        if (list->data != List_data_ptr)
        {
            error_report(LIST_DATA_PTR_CHANGED);
            return -1;
        }

    #endif 

    free(list->data);
    list->data = new_data_ptr;

    #ifdef LIST_PTR_CNTRL

        update_list_pointers_values(list);

    #endif 

    for (unsigned counter = 1 ; counter < list->capacity; counter++) {

        if (counter != list->size && counter != list->capacity - 1)
            list->next[counter] = (int)counter + 1;
        else  
            list->next[counter] = 0;

        if (counter <= list->size) 
            list->prev[counter] = (int)counter - 1;
        else 
            list->prev[counter] = -1;
    }

    list->is_linearized = 1;

    if (list->free != 0)
        list->free = list->size + 1;

    list->head = 1;
    list->tail = list->size;

    #ifdef LIST_HASH

        int ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    return 0;
}

//===================================================================

static int _list_clear_check(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    int is_list_clear = is_memory_clear((void*)list, 
                                        sizeof(struct List), 
                                        sizeof(char));
                                    
    return is_list_clear;
}

//===================================================================

static int _list_poison_check(struct List* list FOR_LOGS(, LOG_PARAMS)) {
    
    list_log_report();
    LIST_POINTER_CHECK(list);

    for (long unsigned counter = 0; 
                       counter < sizeof(List); counter++) {

        if (*((unsigned char*)list + counter) != LIST_POISON_VALUE)
            
            return 0;
    }

    return 1;
}

//===================================================================

static int _list_poisoning(struct List* list FOR_LOGS(, LOG_PARAMS)) {
    
    list_log_report();
    LIST_POINTER_CHECK(list);

    for (long unsigned counter = 0; 
                       counter < sizeof(List); counter++) {

        *((char*)list + counter) = (char)LIST_POISON_VALUE;
    }

    return 0;
}

//===================================================================

static int _list_set_zero(struct List* list FOR_LOGS(, LOG_PARAMS)) {
    
    list_log_report();
    LIST_POINTER_CHECK(list);

    int clear_return = clear_memory((void*)list, 
                                    sizeof(List), 
                                    sizeof(char));

    if (clear_return == -1)
        return -1;

    return 0;
}

//===================================================================

#ifdef LIST_PTR_CNTRL

    static int _update_list_pointers_values(struct List* list FOR_LOGS(, LOG_PARAMS)) {

        list_log_report();
        LIST_POINTER_CHECK(list);

        extern void* List_data_ptr;
        List_data_ptr = list->data;

        extern void* List_next_ptr;
        List_next_ptr = list->next;

        extern void* List_prev_ptr;
        List_prev_ptr = list->prev;

        return 0;
    }

#endif 

//===================================================================

static int _list_check_free_elements(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    unsigned free_amount = (unsigned)list->capacity - 1 - list->size;

    unsigned free = list->free;
    unsigned counter = 0;

    while (free != 0) {

        if (list->prev[free] != -1) 
        {
            error_report(LIST_PREV_IS_INCORRECT);
            return 0;
        }

        counter++;

        if (counter > free_amount)
        {
            error_report(LIST_INVALID_FREE_AMOUNT);
            return 0;
        }

        free = (unsigned)list->next[free];
    }

    if (counter != free_amount) {

        error_report(LIST_INVALID_FREE_AMOUNT);
        return 0;
    }

    return 1;
}

//===================================================================

static int _list_check_connections(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->head == 0 
     && list->tail == 0 
     && list->size == 0)
        return 1;

    if (list->head > list->capacity - 1 
     || list->tail > list->capacity - 1 
     || (list->size != 0 && (list->tail == 0 || list->head == 0)))
        
        return 0;

    if (list->prev[list->head] != 0 || list->next[list->tail] != 0) {

        error_report(LIST_CONNECTIONS_RUINED);
        return 0;
    }
    
    unsigned index = list->head;
    unsigned counter = 0;

    while (index != list->tail) {

        if (index != list->head
         && list->next[list->prev[index]] != (int)index) {

            error_report(LIST_CONNECTIONS_RUINED);
            return 0;
        }

        if (index != list->tail
         && list->prev[list->next[index]] != (int)index) {

             error_report(LIST_CONNECTIONS_RUINED);
             return 0;
        }

        if (list->prev[index] == -1) {

            error_report(LIST_PREV_IS_INCORRECT);
            return 0;
        }

        counter++;
        index = (unsigned)list->next[index];

        if (counter > list->size) {

            error_report(LIST_ELEMENT_LOOP);
            return 0;
        }
    }
    
    return 1;
}

//===================================================================

#ifdef LIST_PTR_CNTRL

    static int _list_pointers_values_check(struct List* list FOR_LOGS(, LOG_PARAMS)) {

        list_log_report();
        LIST_POINTER_CHECK(list);

        extern void* List_data_ptr;

        if (list->data != (list_elem_t*)List_data_ptr) {

            error_report(LIST_DATA_PTR_CHANGED);
            return 0;
        }

        if (list->data == NULL) {

            error_report(LIST_DATA_IS_NULL);
            return -1;
        }

        extern void* List_next_ptr;

        if (list->next != (int*)List_next_ptr) {

            error_report(LIST_NEXT_PTR_CHANGED);
            return 0;
        }

        if (list->next == NULL) {

            error_report(LIST_NEXT_IS_NULL);
            return -1;
        }

        extern void* List_prev_ptr;

        if (list->prev != (int*)List_prev_ptr) {

            error_report(LIST_PREV_PTR_CHANGED);
            return 0;
        }

        if (list->prev == NULL) {

            error_report(LIST_PREV_IS_NULL);
            return -1;
        }

        return 1;
    }

#endif 

//===================================================================

static int _list_allocate_memory(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    list_elem_t* data_temp = (list_elem_t*)calloc(list->capacity, 
                                        sizeof(list_elem_t));

    if (data_temp == NULL)
        error_report(CANNOT_ALLOCATE_MEM);
    else 
        list->data = data_temp;

    int* next_temp = (int*)calloc(list->capacity, sizeof(int));

    if (next_temp == NULL)
        error_report(CANNOT_ALLOCATE_MEM);
    else    
        list->next = next_temp;

    int* prev_temp = (int*)calloc(list->capacity, sizeof(int));

    if (prev_temp == NULL)
        error_report(CANNOT_ALLOCATE_MEM);
    else 
        list->prev = prev_temp;

    #ifdef LIST_PTR_CNTRL

        int ret = update_list_pointers_values(list);
        if (ret == -1)
            return -1;

    #endif 

    return 0;
    
}

//===================================================================

static int _list_free_memory(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifdef LIST_PTR_CNTRL

        int is_ok = list_pointers_values_check(list);
        if (is_ok == 0)
            return -1;

    #endif 

    free(list->data);
    free(list->prev);
    free(list->next);

    return 0;
}

//===================================================================

int _list_dtor(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifdef LiST_VALID_CHECK

        int is_ok = list_validator(list);
        if(is_ok == 0)
            return -1;

    #endif 

    #ifdef LIST_PTR_CNTRL

        int is_pointers_ok = list_pointers_values_check(list);
        if (is_pointers_ok == 0)
            return -1;

    #endif 

    int ret = list_free_memory(list);
    if (ret == -1)
        return -1;

    #ifndef LIST_FAST

        list_poisoning(list);

    #endif 

    return 0;
}

//===================================================================

static int _list_set_prev_to_minus_one(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->prev == NULL) {

        error_report(LIST_PREV_IS_NULL);
        return -1;
    }

    for (long unsigned counter = 1;  
                       counter < list->capacity; counter++) {

        list->prev[counter] = -1;
    }

    list->prev[0] = 0;

    return 0;
}

//===================================================================

static int _list_set_next_in_order(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->next == NULL) {

        error_report(LIST_NEXT_IS_NULL);
        return -1;
    }

    for (long unsigned counter = 1;  
                       counter < list->capacity; counter++) {

        list->next[counter] = (int)counter + 1;
    }

    list->next[list->capacity - 1] = 0;
    list->next[0] = 0;
    
    return 0;
}

//===================================================================

int _list_ctor(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifndef LIST_FAST

        int is_poison = list_poison_check(list);
        int is_clear = list_clear_check(list);

        if (is_clear == 0 && is_poison == 0) {
            
            error_report(LIST_UNPREPARED);
            return -1;
        }
        if (is_poison == 1) {

            int ret = list_set_zero(list);

            if (ret == -1)
                return -1;
        }

        if (List_max_capacity < List_start_capacity) 
        {
            error_report(LIST_INV_MAX_CAPACITY);
            return -1;
        }

    #endif 
    
    list->head = 0;
    list->tail = 0;
    list->free = 1;

    list->is_linearized = 1;

    list->capacity = List_start_capacity;
    list->size = 0;
    
    int ret = list_allocate_memory(list);
    if (ret == -1)
        return -1;

    ret = list_set_next_in_order(list);
    if (ret == -1)
        return -1;

    ret = list_set_prev_to_minus_one(list);
    if (ret == -1)
        return -1;
        
    #ifdef LIST_DUMP

        list->self_ptr = list;

    #endif
    
    #ifdef LIST_HASH
    
        ret =  list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if(is_ok == 0)
            return -1;

    #endif 

    return 0;
}

//===================================================================

int _list_validator(struct List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    int err_val = 0;

    #ifdef LIST_PTR_CNTRL

        if (list_pointers_values_check(list) == 0) {

            err_val++;
        }

    #endif 

    #ifdef LIST_HASH

        if (list_hash_check(list) == 0) {

            err_val++;
        }

    #endif

    #ifdef LIST_DUMP

        if (list != list->self_ptr) {

            error_report(LIST_INV_SELF_PTR);
            err_val++;
        }

    #endif

    if  (list->data == NULL) {

        error_report(LIST_DATA_IS_NULL);
        err_val++;
    }

    if (list->next == NULL) {

        error_report(LIST_NEXT_IS_NULL);
        err_val++;
    }

    if (list->head > list->capacity - 1) {

        error_report(LIST_INV_HEAD);
        err_val++;
    }

    if (list->tail > list->capacity - 1) {

        error_report(LIST_INV_HEAD);
        err_val++;
    }

    if (list->head == list->tail && list->size > 1) {

        error_report(TAIL_EQUAL_HEAD);
        err_val++;
    }
    
    if (list->head == 0 && list->size != 0) {

        error_report(LIST_INV_HEAD);
        err_val++;
    }

    if (list->tail == 0 && list->size != 0) {

        error_report(LIST_INV_TAIL);
        err_val++;
    }

    if (list->size > list->capacity - 1) {

        error_report(INV_LIST_SIZE);
        err_val++;
    }

    if (list->size > list->capacity - 1) {

        error_report(SIZE_MORE_THAN_CAP);
        err_val++;
    }

    if (list->is_linearized != 0 && list->is_linearized != 1) {

        error_report(INV_IS_LINEARIZED_VALUE);
        err_val++;
    }

    if (list->next != NULL && list->tail < list->capacity) {

        if (list->next[list->tail] != 0) {

            error_report(LIST_INV_TAIL_NEXT);
            err_val++;
        }
    }

    if (list->prev != NULL && list->head < list->capacity) {

        if (list->prev[list->head] != 0) {

            error_report(LIST_INV_HEAD_PREV);
            err_val++;
        }
    }

    if (list_check_connections(list) != 1) {

        err_val++;
    }

    if (list_check_free_elements(list) != 1) {

        err_val++;
    }

    #ifdef LIST_DUMP

        if (list != NULL) {
            int ret = list_dump(list, logs_file);
            
            if (ret == -1)
                return -1;
        }

    #endif

    if (err_val != 0) {

        #ifdef LIST_LOGS

            fflush(logs_file);

        #endif 

        return 0;
    }

    return 1;
}

//===================================================================

static int _list_out(struct List* list, FILE* output FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    if (list->data == NULL || list->next == NULL)
        return -1;

    fprintf(output, "<style> td\n { padding: 7px;"
                                  " border: 1px solid black;"
                                  " border-collapse: collapse;}\n</style>");

    fprintf(output, "<table width = \" 100%% \" "
                           "cellspacing=\"0\" "
                           "cellpadding=\"4\" "
                           "border = \"5\" "
                           "style = \" "
                           "padding: 15px; "
                           "background-color: lightgrey;>\"\n");

    fprintf(output, "<tr><td><b>Index</b></td>");

    for (unsigned counter = 0; counter < list->capacity; counter++) {
        
        if (list->prev[counter] == -1)
            fprintf(output, "<td><u><b> %u </b></u></td>", counter);
        else 
            fprintf(output, "<td><b> %u </b></td>", counter);
    }
    fprintf(output, "</tr>");

    fprintf(output, "<tr><td><b>Data</b></td>");

    for (unsigned counter = 0; counter < list->capacity; counter++) {
        
        if (list->prev[counter] == -1)
            fprintf(output, "<td><u>" LIST_ELEM_SPEC "</u></td>", list->data[counter]);
        else
            fprintf(output, "<td>" LIST_ELEM_SPEC "</td>", list->data[counter]);
    }

    fprintf(output, "</tr>");

    fprintf(output, "<tr><td><b>Next</b></td>");

    for (unsigned counter = 0; counter < list->capacity; counter++) {
        
        if (list->prev[counter] == -1)
            fprintf(output, "<td><u> %d </u></td>", list->next[counter]);
        else
            fprintf(output, "<td> %d </td>", list->next[counter]);
    }

    fprintf(output, "</tr>");

    fprintf(output, "<tr><td><b>Prev</b></td>");

    for (unsigned counter = 0; counter < list->capacity; counter++) {
        
        if (list->prev[counter] == -1)
            fprintf(output, "<td><u> %d </u></td>", list->prev[counter]);
        else
            fprintf(output, "<td> %d </td>", list->prev[counter]);
    }
    fprintf(output, "</tr>");

        fprintf(output, "</table>");

    return 0;
}

//===================================================================

int _list_dump(struct List* list, FILE* output FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (output == NULL) {
       
        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(output, "\n <table style = \"border-collapse: collapse; "
                                        "border: 3px solid black\"  "
                                        "class = \"table\">\n");

    fprintf(output, "<caption style=\"border-collapse: collapse;   "
                                     "background-color: lightgrey; "
                                     "border: 2px solid black;\">  "
                                     "<b>Double linked list structure.</b>"
                                                             "</caption>");

    fprintf(output, "<tr><td>\nType</td><td> %s.</td></tr>", LIST_TYPE_NAME); 
    fprintf(output, "<tr><td>Address</td><td> <%p></td></tr>\n", list);

    fprintf(output, "<tr><td>List linerized</td><td> %d </td></tr>\n",
                                                 list->is_linearized);

    fprintf(output, "<tr><td>List capacity</td><td>%lu</td></tr>\n", 
                                                    list->capacity);

    fprintf(output, "<tr><td>List size</td><td>%u</td></tr>\n", 
                                                   list->size);

    fprintf(output, "<tr><td>Data array address</td><td><%p></td></tr>\n", 
                                                              list->data);

    fprintf(output, "<tr><td>Next element indexes array address</td><td><%p></td></tr>\n", 
                                                                              list->next);

    fprintf(output, "<tr><td>Previous element indexes array address</td><td><%p></td></tr>\n", 
                                                                                  list->prev);

    fprintf(output, "<tr><td>Head element</td><td>%u</td></tr>\n", 
                                                      list->head);

    fprintf(output, "<tr><td>Tail element</td><td>%u</td></tr>\n", 
                                                      list->tail);

    fprintf(output, "<tr><td>First free element</td><td>%u</td></tr>\n", 
                                                            list->free);

    fprintf(output, "\n</table>\n");

    if (list->data == NULL)
        fprintf(output, "ERROR: NULL data array\n");
        
    if (list->next == NULL)    
        fprintf(output, "ERROR: NULL next index array\n");

    if (list->prev == NULL)
        fprintf(output, "ERROR: NULL previous index array\n");

    if (list->head > list->capacity - 1 || (list->head == 0 && list->size != 0))
        fprintf(output, "ERROR: invalid head element\n");

    if (list->tail > list->capacity - 1 || (list->tail == 0 && list->size != 0))
        fprintf(output, "ERROR: invalid tail element\n");

    if (list->head == list->tail && list->size > 1)
        fprintf(output, "ERROR: Tail equal to head while "
                                "size is mote than one\n");

    if (list->free > list->capacity - 1)
        fprintf(output, "ERRROR: invalid list->free");

    if (list->prev[list->free] != -1)
        fprintf(output, "Prev for list->free is not -1. "
                     "No free elements left in list.\n");


    int ret = list_out(list, output);
    if (ret == -1)            
        return -1;

    #ifdef LIST_GRAPHVIZ

        ret = list_draw_graph(list);
        if (ret == -1)
            return -1;

        ret = list_draw_graph_logical(list);
        if (ret == -1)
            return -1;

    #endif

    return 0;
}

//===================================================================

int _list_push_first(struct List* list, list_elem_t value, int free 
                                        FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->head != 0 
     || list->tail != 0 
     || list->size != 0) {

        error_report(PUSH_FIRST_FALSE_CALL);
        return -1;
    }

    list->data[free] = value;
    list->next[free] = 0;
    list->prev[free] = 0; 

    list->head = (unsigned)free;
    list->tail = (unsigned)free;

    list->size++;

    #ifdef LIST_HASH
    
        int ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    return free;
}

//===================================================================

list_elem_t _list_pop_last(struct List* list, int* err FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    
    if (!list)
    {
        error_report(INV_LIST_STRUCTURE_PTR);
        return NULL;
    }

    if (list->head != list->tail || list->size != 1) {

        error_report(POP_LAST_FALSE_CALL);
        
        *err = -1;
        return NULL;
    }

    unsigned int last = list->tail;

    list->next[last] = (int)list->free;
    list->free = 1;

    list->prev[last] = -1;
    
    list_elem_t value = list->data[last];
    int ret = clear_memory(&list->data[last], 1, sizeof(list_elem_t));
    
    if (ret != 1) {

        *err = -1;
        return NULL;
    }

    list->head = 0;
    list->tail = 0;

    list->size--;

    list->is_linearized = 1;

    ret = list_set_next_in_order(list);
    if (ret == -1)
        return NULL;

    #ifdef LIST_HASH

        ret = list_save_hash(list);
        if (ret == -1)
            return NULL;

    #endif

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0) 
        {
            *err = -1;
            return NULL;
        }

    #endif 

    return value;
}

//===================================================================

int _list_get_free(struct  List* list FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    if (list->size == list->capacity - 1) {

        error_report(LIST_OVERFLOW);
        return -1;
    }

    unsigned free = list->free;
    list->free = (unsigned)list->next[free];

    #ifdef LIST_HASH

        int ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif 

    if (free > list->capacity - 1) {

        error_report(LIST_INV_FREE);
        return -1;
    }    

    if (list->prev[free] != -1)  {

        error_report(LIST_INV_PREV_FOR_FREE_ELEM);
        return -1;
    }

    return (int)free;
}

//===================================================================

static int _list_push_check(struct List* list, unsigned int index  
                                           FOR_LOGS(,LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    if (index == 0 && list->size != 0) {

        error_report(LIST_PUSH_BY_ZERO);
        return 0;
    }

    if (index > (unsigned)list->capacity) {

        error_report(LIST_INVALID_INDEX);
        return 0;
    }

    if (list->prev[index] == -1) { 

        error_report(LIST_EMPTY_INDEX);
        return 0;
    }

    if (list->size == List_max_capacity - 1) {

        error_report(LIST_OVERFLOW);
        return 0;
    }

    return 1;
}

//===================================================================

int _list_push_before_index(struct List* list, unsigned int index, list_elem_t value 
                                                         FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifndef LIST_FAST

        int is_ok = list_push_check(list, index);
        if (is_ok == 0)
            return -1;

    #endif 

    if (list->size == list->capacity - 1) {

        int ret = list_increase(list);
        if (ret == -1)
            return -1;
    }

    int free = list_get_free(list);
    if (free == -1)
        return -1;

    if (list->head == 0
     && list->tail == 0 
     && list->size == 0) {

        int ret = list_push_first(list, value, free);
        if (ret == -1)
            return -1;

        return free;
    }   

    if (index == list->head && (unsigned)free > list->head) 
        list->is_linearized = 0; 

    list->data[free] = value;

    if (index != list->head 
    || (index == list->head && free > (int)list->head))

        list->is_linearized = 0;

    list->data[free] = value;
    list->next[free] = (int)index;
    list->prev[free] = list->prev[index];

    if (list->prev[index] != 0)
        list->next[list->prev[index]] = free;
    
    list->prev[index] = free;

    if (list->prev[free] == 0) 
        list->head = (unsigned)free;

    list->size++;

    #ifdef LIST_HASH

        int ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    #ifdef LIST_VALID_CHECK

        is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    return free;
}

//===================================================================

int _list_push_after_index(struct List* list, unsigned int index, list_elem_t value 
                                                        FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifndef LIST_FAST

        int is_ok = list_push_check(list, index);
        if (is_ok == 0)
            return -1;

    #endif 

    if (list->size == list->capacity - 1) {

        int ret = list_increase(list);
        if (ret == -1)
            return -1;
    }

    int free = list_get_free(list);
    if (free == -1)
        return -1;

    if (list->head == 0 
     && list->tail == 0 
     && list->size == 0) {

        int ret = list_push_first(list, value, free);
        if (ret == -1)
            return -1;

        return free;
    }

    if (index != list->tail)
        list->is_linearized = 0;

    list->data[free] = value;
    list->prev[free] = (int)index;
    list->next[free] = list->next[index];

    if (list->next[index] != 0)
        list->prev[list->next[index]] = free;
    
    list->next[index] = free;

    if (list->next[free] == 0) 
        list->tail = (unsigned)free;

    list->size++;

    #ifdef LIST_HASH

        int ret = list_save_hash(list);
        if (ret == -1)
            return -1;

    #endif

    #ifdef LIST_VALID_CHECK

        is_ok = list_validator(list);
        if (is_ok == 0)
            return -1;

    #endif 

    return free;
}

//===================================================================

static int _list_pop_check(struct List* list, unsigned int index
                                           FOR_LOGS(,LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    #ifdef LIST_VALID_CHECK

        int is_ok = list_validator(list);
        if (is_ok == 0) 
        {
            return -1;
        }

    #endif 

     if (list->size == 0) {

        error_report(LIST_UNDERFLOW);
        return 0;
    }

    if (index == 0) {

        error_report(LIST_POP_BY_ZERO);
        return 0;
    }

    if (list->prev[index] == -1) {        

        error_report(LIST_EMPTY_INDEX);
        return 0;
    }

    return 1;
}

//===================================================================

list_elem_t _list_pop_by_index(struct List* list, unsigned int index, int* err 
                                                  FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    
    if (!list)
    {
        error_report(INV_LIST_STRUCTURE_PTR);
        return NULL;
    }

    #ifndef LIST_FAST 

        int is_ok = list_pop_check(list, index);
        if (is_ok == 0) {

            *err = -1;
            return NULL;
        }

    #endif 

    if (list->is_linearized == 1
    && list->head == 1
    && list->size * 4 < list->capacity) {

        int ret = list_decrease(list);
        if (ret == -1)
            return NULL;
    }

    if (list->head == list->tail && list->size == 1) {

        list_elem_t value = list_pop_last(list, err);
        
        if (*err == -1)
            return NULL;
        else
            return value;
    }

    if (index != list->tail && index != list->head)
        list->is_linearized = 0;

    list_elem_t value = list->data[index];

    int ret = clear_memory(&list->data[index], 1, sizeof(list_elem_t));
    if (ret != 1)
        return NULL;

    if (list->prev[index] == 0)  {
        
        list->head = (unsigned)list->next[index];
        list->prev[list->next[index]] = 0;
    }

    else {

        list->next[list->prev[index]] = list->next[index];
        
        if (list->next[index] == 0) 
            list->tail = (unsigned)list->prev[index];
        else 
            list->prev[list->next[index]] = list->prev[index];   
    }

    list->next[index] = (int)list->free;
    list->free = index;

    list->prev[index] = -1;
    list->size--;

    #ifdef LIST_HASH

        ret = list_save_hash(list);
        if (ret == -1)
            return NULL;

    #endif

    #ifdef LIST_VALID_CHECK

        is_ok = list_validator(list);
        if (is_ok == 0) 
        {
            *err = -1;
            return NULL;
        }

    #endif 

    return value;
}

//===================================================================

int _list_push_back(struct List* list, list_elem_t value FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    int ret = list_push_after_index(list, list->tail, value);
    if (ret == -1)
        return -1;

    return ret;
}

//===================================================================

list_elem_t _list_pop_back(struct List* list, int* err FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    
    if(!list)
    {
        error_report(INV_LIST_STRUCTURE_PTR);
        return NULL;
    }

    list_elem_t value = list_pop_by_index(list, list->tail, err);
    if (*err == -1)
        return NULL;

    return value;
}

//===================================================================

list_elem_t _list_pop_front(struct List* list, int* err FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    
    if (!list)
    {
        error_report(INV_LIST_STRUCTURE_PTR);
        return NULL;
    }

    list_elem_t value = list_pop_by_index(list, list->head, err);
    if (*err == -1)
        return NULL;
    
    return value;
}

//===================================================================

int _list_push_front(struct List* list, list_elem_t value FOR_LOGS(, LOG_PARAMS)) {

    list_log_report();
    LIST_POINTER_CHECK(list);

    int ret = list_push_before_index(list, list->head, value);
    if (ret == -1)
        return -1;

    return ret;
}

//===================================================================

