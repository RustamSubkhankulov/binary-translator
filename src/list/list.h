#pragma once

#include "list_config.h"
#include "../logs/errors_and_logs.h"

//===============================================

#ifdef LIST_LOGS

    #ifdef LIST_ONLY_DUMPS

        #define list_log_report() ""

    #else 

        #define list_log_report() \
                log_report()

    #endif 

#else 

    #define list_log_report() ""

#endif 

//===============================================

struct List 
{
    list_elem_t* data;
    int* next;
    int* prev;

    unsigned int head;
    unsigned int tail;
    unsigned int free;

    size_t capacity;
    unsigned int size;

    int is_linearized;

    #ifdef LIST_DUMP

        struct List* self_ptr;

    #endif

    #ifdef LIST_HASH
       
       int64_t base_hash;

       int64_t data_hash;
       int64_t next_hash;
       int64_t prev_hash;

    #endif
};

//==================================================================

#define LIST_POINTER_CHECK(list) {                                  \
                                                                    \
    if (list == NULL) {                                             \
                                                                    \
        error_report(INV_LIST_STRUCTURE_PTR);                       \
        return -1;                                                  \
    }                                                               \
}

//==================================================================

#define update_list_pointers_values(list) \
       _update_list_pointers_values(list FOR_LOGS(, LOG_ARGS)) 

#define list_search(list, elem) \
       _list_search(list, elem FOR_LOGS(, LOG_ARGS))

#define list_ctor(list) \
        _list_ctor(list FOR_LOGS(, LOG_ARGS))

#define list_dtor(list) \
        _list_dtor(list FOR_LOGS(, LOG_ARGS))

#define list_resize(list) \
       _list_resize(list FOR_LOGS(, LOG_ARGS))

#define list_increase(list) \
       _list_increase(list FOR_LOGS(, LOG_ARGS))

#define list_set_zero(list) \
       _list_set_zero(list FOR_LOGS(, LOG_ARGS))

#define list_get_free(list) \
       _list_get_free(list FOR_LOGS(, LOG_ARGS))

#define list_decrease(list) \
       _list_decrease(list FOR_LOGS(, LOG_ARGS))

#define list_save_hash(list) \
       _list_save_hash(list FOR_LOGS(, LOG_ARGS))

#define list_poisoning(list) \
       _list_poisoning(list FOR_LOGS(, LOG_ARGS))

#define list_linearize(list) \
       _list_linearize(list FOR_LOGS(, LOG_ARGS))

#define list_hash_check(list) \
       _list_hash_check(list FOR_LOGS(, LOG_ARGS))

#define list_draw_graph(list) \
       _list_draw_graph(list FOR_LOGS(, LOG_ARGS))

#define list_validator(list) \
        _list_validator(list FOR_LOGS(, LOG_ARGS))

#define list_free_memory(list) \
       _list_free_memory(list FOR_LOGS(, LOG_ARGS))

#define list_out(list, output) \
       _list_out(list, output FOR_LOGS(, LOG_ARGS))

#define list_clear_check(list) \
       _list_clear_check(list FOR_LOGS(, LOG_ARGS))

#define list_poison_check(list) \
       _list_poison_check(list FOR_LOGS(, LOG_ARGS))

#define list_pop_back(list, err) \
       _list_pop_back(list, err FOR_LOGS(, LOG_ARGS)) 

#define list_dump(list, output) \
       _list_dump(list, output FOR_LOGS(, LOG_ARGS))

#define list_pop_last(list, err) \
       _list_pop_last(list, err FOR_LOGS(,LOG_ARGS))

#define list_pop_front(list, err) \
       _list_pop_front(list, err FOR_LOGS(, LOG_ARGS))

#define list_allocate_memory(list) \
       _list_allocate_memory(list FOR_LOGS(, LOG_ARGS))

#define list_pop_check(list, index) \
       _list_pop_check(list, index FOR_LOGS(, LOG_ARGS))

#define list_push_back(list, value) \
       _list_push_back(list, value FOR_LOGS(, LOG_ARGS))  

#define list_push_front(list, value) \
       _list_push_front(list, value FOR_LOGS(, LOG_ARGS))

#define list_push_check(list, index) \
       _list_push_check(list, index FOR_LOGS(, LOG_ARGS))

#define list_check_connections(list) \
       _list_check_connections(list FOR_LOGS(, LOG_ARGS))

#define list_set_next_in_order(list) \
       _list_set_next_in_order(list FOR_LOGS(, LOG_ARGS))

#define list_push_front(list, value) \
       _list_push_front(list, value FOR_LOGS(, LOG_ARGS))

#define list_draw_graph_logical(list) \
       _list_draw_graph_logical(list FOR_LOGS(, LOG_ARGS))

#define list_check_free_elements(list) \
       _list_check_free_elements(list FOR_LOGS(, LOG_ARGS))

#define list_pointers_values_check(list) \
       _list_pointers_values_check(list FOR_LOGS(, LOG_ARGS))

#define list_set_prev_to_minus_one(list) \
       _list_set_prev_to_minus_one(list FOR_LOGS(, LOG_ARGS))

#define list_push_first(list, value, free) \
       _list_push_first(list, value, free FOR_LOGS(, LOG_ARGS))  

#define list_pop_by_index(list, index, err) \
       _list_pop_by_index(list, index, err FOR_LOGS(, LOG_ARGS))

#define get_prev_element_index(list, index) \
       _get_prev_element_index(list, index FOR_LOGS(, LOG_ARGS))

#define list_show_by_index(list, index, err) \
       _list_show_by_index(list, index, err FOR_LOGS(, LOG_ARGS))

#define list_push_after_index(list, index, value) \
       _list_push_after_index(list, index, value FOR_LOGS(, LOG_ARGS))

#define list_push_before_index(list, index, value) \
       _list_push_before_index(list, index, value FOR_LOGS(, LOG_ARGS))

#define list_recalloc_buffers(list, prev_capacity) \
       _list_recalloc_buffers(list, prev_capacity FOR_LOGS(, LOG_ARGS))

#define list_prepare_after_increase(list, prev_cap) \
       _list_prepare_after_increase(list, prev_cap FOR_LOGS(, LOG_ARGS))

#define list_get_by_logical_number(list, number, err) \
       _list_get_by_logical_number(list, number, err FOR_LOGS(, LOG_ARGS))

//===================================================================

int _list_get_free(struct  List* list FOR_LOGS(, LOG_PARAMS));

list_elem_t _list_pop_last(struct List* list, int* err FOR_LOGS(, LOG_PARAMS));  

int _list_push_first(struct List* list, list_elem_t value, int free 
                                        FOR_LOGS(,LOG_PARAMS));   

//===================================================================

int _list_ctor(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_dtor(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_linearize(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_validator(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_draw_graph(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_draw_graph_logical(struct List* list FOR_LOGS(, LOG_PARAMS));

int _list_dump(struct List* list, FILE* output FOR_LOGS(, LOG_PARAMS));

//===================================================================

// int _list_search(struct List* list, list_elem_t elem FOR_LOGS(, LOG_PARAMS));

list_elem_t _list_pop_back(struct List* list, int* err FOR_LOGS(, LOG_PARAMS));

list_elem_t _list_pop_front(struct List* list, int* err FOR_LOGS(, LOG_PARAMS));

int _list_push_back(struct List* list, list_elem_t value FOR_LOGS(, LOG_PARAMS));

int _list_push_front(struct List* list, list_elem_t value FOR_LOGS(, LOG_PARAMS));

int _list_push_after_index(struct List* list, unsigned int index, 
                                       list_elem_t value FOR_LOGS(, LOG_PARAMS));

list_elem_t _list_pop_by_index(struct List* list, unsigned int index,  
                                           int* err FOR_LOGS(, LOG_PARAMS));

int _list_get_by_logical_number(struct List* list, int number, 
                                           int* err FOR_LOGS(, LOG_PARAMS));

int _list_push_before_index(struct List* list, unsigned int index,
                                        list_elem_t value FOR_LOGS(, LOG_PARAMS));


