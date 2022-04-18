#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "stack.h"
#include "../../logs/errors_and_logs.h"

//===================================================================

int stack_save_hash_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    #ifdef HASH

        int base_hash_size = sizeof(struct Stack) - 2 * sizeof(int64_t);

        int64_t hash = get_hash((char*)stack, 
                                (unsigned)base_hash_size);
        stack->base_hash = hash;

        if (stack->data != (elem_t*)(DEFAULT_PTR)
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != NULL) {

            int data_hash_size = stack->capacity 
                               * stack->size_of_elem;

            hash = get_hash((char*)stack->data, 
                            (unsigned)data_hash_size);
            stack->data_hash = hash;
        }

    #endif

    return 0;
}

//===================================================================

#ifdef HASH

  int stack_data_hash_check_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

        if (stack->data != NULL
            && stack->data != (elem_t*)DEFAULT_PTR
            && stack->data != (elem_t*)POISON_PTR) {

            int size = stack->capacity 
                     * stack->size_of_elem;

            int64_t hash = get_hash((char*)stack->data, 
                                    (unsigned)size);

            if (stack->data_hash == hash)
                return 1;
            else
                return 0;
        }

    return 0;
}

#endif

//===================================================================

#ifdef HASH

  int stack_base_hash_check_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    unsigned hash_size = (unsigned)(sizeof(struct Stack)
                                     - 2 * sizeof(int64_t));

    int64_t hash = get_hash((char*)stack, hash_size);

    if (stack->base_hash == hash)
        return 1;
    else
        return 0;
}

#endif

//===================================================================

   int stack_set_canaries_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    int64_t* canary_ptr = (int64_t*)stack->data - 1;

    *canary_ptr = Canary_val;

    int canary_offset = stack->capacity * (int)sizeof(elem_t);

    canary_ptr = (int64_t*)((char*)stack->data + canary_offset);

    *canary_ptr = Canary_val;

    return 0;
}

//===================================================================

int clear_mem_(void* base, int n, int size, LOG_PARAMS) {

    stack_log_report();

    if (base == NULL || n < 1 || size < 1) 
        error_report(CLEAR_MEM_CHECK_ERR);

    memset(base, 0, (long unsigned)(n * size));

    return clear_mem_check(base, n, size);
}

//===================================================================

int clear_mem_check_(void* base, int n, int size, LOG_PARAMS) {

    stack_log_report();

    if (base == NULL || n < 0 || size < 0) 
         error_report(CLEAR_MEM_CHECK_ERR);       

    char* base_char = (char*)base;
    int char_n = n * size;

    for (int char_ct = 0; char_ct < char_n; char_ct++) {

        if (base_char[char_ct] != 0) 
            return 0; 
    }

    return 1;
}

//===================================================================

   int stack_free_data_check_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack->data == NULL
        || stack->data == (elem_t*)DEFAULT_PTR
        || stack->data == (elem_t*)POISON_PTR) {

        error_report(STK_INV_DATA_PTR);
        return -1;
    }

    int size      = stack->size_of_elem;
    int number    = stack->capacity - stack->count;
    void* pointer = &stack->data[stack->count];

    return clear_mem_check(pointer, number, size);
}

//===================================================================

#ifdef CANARIES

  int _stack_canary_validation(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();

    int check_val = 0;

    if (stack->canary1 != Canary_val) {

        error_report(STK_CANARY1_ERR);
        check_val++;
    }

    if (stack->canary2 != Canary_val) {

        error_report(STK_CANARY1_ERR);
        check_val++;
    }

    if (stack->data != NULL
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->data != (elem_t*)POISON_PTR) {

        int64_t* canary_ptr =(int64_t*)stack->data - 1;

        if (*canary_ptr != Canary_val) {

            error_report(STK_DATA_CANARY1_ERR);
            check_val++;
        }

        int offset = stack->capacity * (int)sizeof(elem_t);

        canary_ptr =(int64_t*)((char*)stack->data + offset);

        if (*canary_ptr != Canary_val) {

            error_report(STK_DATA_CANARY2_ERR);
            check_val++;
        }
    }

    if (check_val != 0)
        return 1;
    else
        return 0;
}

#endif

//===================================================================

int stack_validator_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    int check_val = 0;

    #ifdef HASH

        if (stack_base_hash_check(stack) == 0) {

            error_report(STK_BASE_HASH_ERR);
            check_val++;
        }

        if (stack->data != NULL
            && stack->data != (elem_t*)DEFAULT_PTR
            && stack->data != (elem_t*)POISON_PTR
            && stack_data_hash_check(stack) == 0) {

            error_report(STK_DATA_HASH_ERR);
            check_val++;
        }

    #endif

    if (stack->self_ptr != stack) {

        error_report(STK_INV_SELF_PTR);
        check_val++;
    }

    #ifdef CANARIES

        check_val += stack_canary_validation(stack);   
        
    #endif

    if ((stack->capacity > 0 
        && stack->data == (elem_t*)DEFAULT_PTR)
        || stack->data == NULL
        || stack->data == (elem_t*)POISON_PTR) {

        error_report(STK_INV_DATA_PTR);
        check_val++;
    }

    if (strncmp("stack_push_", ___func_name, NAMEBUFSIZE) == 0
        && stack->capacity == MAXCAPACITY
        && stack->count == MAXCAPACITY) {

        error_report(STK_OVRFLW);
        //stack_save_hash(stack);

        return -1;
    }

    if (strncmp("stack_pop_", ___func_name, NAMEBUFSIZE) == 0
        && stack->count == 0
        && stack->capacity == 0) {

        error_report(STK_NO_ELEMS_PUSHED);
        //stack_save_hash(stack);
    }

    if (stack->size_of_elem != sizeof(elem_t)
        || stack->size_of_elem <= 0) {

        error_report(STK_INV_SZ_ELEM);
        check_val++;
    }

    if (stack->capacity < 0) {

        error_report(STK_INV_CAPACITY);
        check_val++;
    }

    if (stack->count < 0) {

        error_report(STK_INV_ELEM_CT);
        check_val++;
    }

    if (stack->capacity < stack->count) {

        error_report(STK_CAPACITY_LESS_CT);
        check_val++;
    }
    if (stack->size_of_elem > 0 
        && stack->count != 0 
        && stack->capacity != 0 
        && stack_free_data_check(stack) == 0) {

        error_report(STK_INV_FREE_DATA);
        check_val++;
    }

    if (check_val > 0) {

        #ifdef DEBUG
            stack_dump_(stack, LOGS_ARGS_USE);
        #endif

        return -1;
    }

    return 1;
}

//===================================================================

void* recalloc_(void* ptr, int number, int prev_number, 
                          int size_of_elem, LOG_PARAMS) {
    stack_log_report();

    if (ptr == NULL) {

        error_report(RECALLOC_INV_POINTER);
        return NULL;
    }

    int new_size = number * size_of_elem;
    void* new_ptr = realloc(ptr, (long unsigned)new_size);

    if (new_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    if (number > prev_number) {

        int offset = number - prev_number;
        char* base = (char*)new_ptr + size_of_elem * prev_number;

        int value = clear_mem(base, offset, size_of_elem);
        
        if (value != 1) 
            return NULL;
    }

    return new_ptr;
}

//===================================================================

   int stack_poison_check_(struct Stack* stack, LOG_PARAMS) {
    
    stack_log_report();
    STACK_PTR_CHECK(stack);

    int check_value = 0;

    check_value += (stack->data  != (elem_t*)POISON_PTR);
    check_value += (stack->count        != POISON_VALUE);
    check_value += (stack->capacity     != POISON_VALUE);
    check_value += (stack->size_of_elem != POISON_VALUE);
    check_value += (stack->error_code   != POISON_VALUE);

    #ifdef HASH

        check_value += (stack->base_hash != POISON_VALUE);
        check_value += (stack->data_hash != POISON_VALUE);
    
    #endif

    #ifdef CANARIES
        
        check_value += (stack->canary1   != POISON_VALUE);
        check_value += (stack->canary2   != POISON_VALUE);
        
    #endif

    if (check_value == 0) return 1;

    return 0;
}

//===================================================================

  int stack_empty_check_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    return clear_mem_check(stack, 1, sizeof(stack));
}

//===================================================================
#ifdef DEBUG

  int stack_set_origin_(struct Stack* stack, LOG_PARAMS) {

    STACK_PTR_CHECK(stack);

    stack->origin.orig_line = ___line;

    strncpy(stack->origin.orig_file_name, 
                               ___file_name, NAMEBUFSIZE);

    strncpy(stack->origin.orig_func_name, 
                               ___func_name, NAMEBUFSIZE);

    return 0;
}

#endif
//===================================================================

int stack_ctor_(struct Stack* stack,  LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack_ctor_valid_check(stack) != 1) {

        error_report(STK_CTOR_UNPREPARED);
        return -1;
    }

    stack_preparing(stack);

    #ifdef DEBUG
        stack_set_origin_(stack, LOGS_ARGS_USE);
    #endif

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    #ifdef DEBUG 

        int ret_val = stack_validator(stack);
        
        if (ret_val == -1)
            return -1;

    #endif 

    return 0;
}

//===================================================================

  int stack_set_size_(struct Stack* stack, int new_size, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack->count > new_size) {

        error_report(STK_SZ_LESS_CT);
        return -1;
    }

    if (new_size < 1) {

        error_report(STK_INV_NEW_SIZE);
        return -1;
    }

    int prev_capacity = stack->capacity;
    stack->capacity = new_size;

    if (stack->count == 0 && prev_capacity == 0) {

        #ifdef CANARIES

            long unsigned size = (long unsigned) (new_size * stack->size_of_elem) 
                               + 2 * sizeof(int64_t);

            elem_t* new_data_ptr = (elem_t*)calloc((long unsigned)size, 
                                                   sizeof(char));

        #else 

            elem_t* new_data_ptr = (elem_t*)calloc(new_size, 
                                        stack->size_of_elem);
        
        #endif

        if (new_data_ptr == NULL) 
            error_report(CANNOT_ALLOCATE_MEM)

        else {

            #ifdef CANARIES

                char* data_ptr = (char*)new_data_ptr 
                                  + sizeof(int64_t);

                stack->data = (elem_t*)data_ptr;
                stack_set_canaries(stack);

            #else    
                stack->data = new_data_ptr;
            #endif

            #ifdef HASH
                stack_save_hash(stack);
            #endif
        }    
    }

    else {

        #ifdef CANARIES

            int64_t* data_ptr = (int64_t*)stack->data - 1;

            long unsigned  size = (long unsigned) new_size * sizeof(elem_t)  
                                + 2 * sizeof(int64_t);

            long unsigned prev_size = (long unsigned) prev_capacity * sizeof(elem_t) 
                                    + sizeof(int64_t);

            elem_t* new_data_ptr = (elem_t*)recalloc(data_ptr, (int)size, 
                                                    (int)prev_size, sizeof(char));
        #else
            elem_t* new_data_ptr = (elem_t*)recalloc(stack->data, new_size,
                                            prev_capacity, sizeof(elem_t));
        #endif

        if (new_data_ptr == NULL) 
            error_report(CANNOT_ALLOCATE_MEM);

        #ifdef CANARIES

            stack->data = (elem_t*)((char*)new_data_ptr + sizeof(int64_t));
            stack_set_canaries(stack);

        #else
            stack->data = new_data_ptr;
        #endif
    }

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    return 0;
}

//==============================================================

  int stack_free_data_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack->data == (elem_t*)DEFAULT_PTR
        || stack->data == (elem_t*)POISON_PTR
        || stack->data == NULL) return -1;

    #ifdef CANARIES
        free((char*)stack->data - sizeof(int64_t));
    #else
        free(stack->data);
    #endif

    return 0;
}

//==============================================================

int stack_dtor_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

    stack_poisoning(stack);

    if (stack_poison_check(stack) == 0) {

        error_report(STK_POISON_CHECK);
        return -1;
    }
    
    return 0;
}

//==============================================================

  int stack_poisoning_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    stack->capacity     = POISON_VALUE;
    stack->count        = POISON_VALUE;
    stack->size_of_elem = POISON_VALUE;

    stack->error_code   = POISON_VALUE;

    #ifdef HASH
        stack->base_hash = POISON_VALUE;
        stack->data_hash = POISON_VALUE;
    #endif    

    #ifdef CANARIES
        stack->canary1 = POISON_VALUE;
        stack->canary2 = POISON_VALUE;
    #endif

    stack_free_data(stack);
    stack->data = (elem_t*)POISON_PTR;

    return 0;
}

//==============================================================

  int stack_preparing_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    stack->data = (elem_t*)DEFAULT_PTR;

    stack->capacity = 0;
    stack->count = 0;
    stack->size_of_elem = sizeof(elem_t);

    stack->error_code = 0;

    #ifdef DEBUG

        #ifdef CANARIES
            stack->canary1 = Canary_val;
            stack->canary2 = Canary_val;
        #endif 

        stack->self_ptr = stack;

        #ifdef HASH
            stack_save_hash(stack);
        #endif

    #endif

    return 0;
}

//==============================================================

  int stack_ctor_valid_check_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack_empty_check(stack) || stack_poison_check(stack)) 
        return 1;
    else 
        return 0;
}

//==============================================================

  int stack_resize_(struct Stack* stack, int param, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    if (stack->capacity == MAXCAPACITY && param == EXPAND) {

        error_report(STK_OVRFLW);
        return -1;
    }

    int prev_capacity = stack->capacity;

    switch (param) {

        case EXPAND: {

            stack->capacity *= 2; break;
        }

        case REDUCE: { 

            stack->capacity /= 2; break; 
        }

        default: {

            error_report(STK_UNKNOWN_RESZ_PARAM);
            return -1;
            }
    }

    if (stack->capacity > MAXCAPACITY) 
        stack->capacity = MAXCAPACITY;

    #ifdef CANARIES

        int64_t* data_ptr = (int64_t*)stack->data - 1;

        long unsigned  size = (long unsigned)stack->capacity * sizeof(elem_t) 
                            + 2 * sizeof(int64_t);
        
        long unsigned  prev_size = (long unsigned)prev_capacity * sizeof(elem_t)
                            + sizeof(int64_t);

        elem_t* new_data_ptr = (elem_t*)recalloc(data_ptr, (int)size, 
                                       (int)prev_size, sizeof(char));

    #else

        elem_t* new_data_ptr = (elem_t*)recalloc(stack->data,
                                                 stack->capacity, 
                                                 prev_capacity,
                                                 stack->size_of_elem);
    #endif

    if (new_data_ptr == NULL) 
        error_report(CANNOT_ALLOCATE_MEM)

    else {

        #ifdef CANARIES

            stack->data = (elem_t*)((int64_t*)new_data_ptr + 1);
            stack_set_canaries(stack);

        #else
            stack->data = new_data_ptr;
        #endif

        #ifdef HASH
            stack_save_hash(stack);
        #endif
    }

    return 0;
}

//==============================================================

int stack_push_(struct Stack* stack, elem_t value, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

    if (stack->capacity == 0 && stack->count == 0) 
        stack_set_size(stack,STACK_STARTING_SIZE);

    if (stack->count + 1 > stack->capacity) {

        int stack_resize_ret = stack_resize(stack, EXPAND);
        if (stack_resize_ret == -1)
            return -1;
    }

    stack->data[stack->count++] = value;

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    #ifdef DEBUG

        ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

    return 0;
}

//===============================================================

  int stack_poped_elem_set_zero_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    clear_mem(stack->data + stack->count, 
              stack->size_of_elem, sizeof(char));

    int is_clear = clear_mem_check(&stack->data[stack->count], 
                                           1, sizeof(elem_t));

    if (is_clear != 1)
        return -1;

    return 0;
}

//===============================================================

elem_t stack_pop_(struct Stack* stack, LOG_PARAMS, int* err) {

    stack_log_report();

    if (stack == NULL) {

        error_report(INV_STACK_PTR);
        if (err) *err = -1;
        return -1;
    }

    if (stack->capacity != 0
        && stack->count == 0) {

        error_report(STK_UNDFLW);
        stack_save_hash(stack);
        return -1;
    }

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1) {

            if (err) *err = -1;
            return -1;
        }

    #endif 

    elem_t returning_value = stack->data[--stack->count];

    int is_clear = stack_poped_elem_set_zero(stack);
    if (is_clear == -1)
        return -1;

    if (stack->count < (stack->capacity / 4)
     && stack->capacity > STACK_STARTING_SIZE) 

        stack_resize(stack, REDUCE);

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    #ifdef DEBUG

        ret_val = stack_validator(stack);
        if (ret_val == -1) {

            if (err) *err = -1;
            return -1;
        }

    #endif 

    return returning_value;
}

//===================================================================

#ifdef DEBUG

  int stack_empty_orig_file_check(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    int value = 0;
    for (int i = 0; i < NAMEBUFSIZE; i++)
        value += stack->origin.orig_file_name[i];

    if (value == 0) return 1;

    return 0;
}

#endif

//===================================================================

#ifdef DEBUG

  int stack_empty_orig_func_check(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    int value = 0;
    for (int i = 0; i < NAMEBUFSIZE; i++)
        value += stack->origin.orig_func_name[i];

    if (value == 0) return 1;

    return 0;
}

#endif

//===================================================================

int stack_out_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    stack_validator(stack);

    fprintf(logs_file, "\n <pre> <div class = \"outline\" style = \"background-color:lightgrey; style = \"text_align: center;>\n");

    fprintf(logs_file, "\n" "Stack <%p> Element type: " TYPE_NAME
                        "\n", stack);

    fprintf(logs_file, "Size of stack element " "%lu" "\n",
                         sizeof(elem_t));

    fprintf(logs_file, "Stack capacity: %d. Number of element in stack %d" "\n\n",
                         stack->capacity, stack->count);

    for (int count = 1; count <= stack->count; count++)

        fprintf(logs_file, "[%d] DEC " ELEM_SPEC "  HEX %x" "\n", count,
                                                 stack->data[count - 1], 
                                  (int)(stack->data[count - 1]));

    fprintf(logs_file, "\n </div> </pre>\n\n");

    return 0;
}

//===================================================================

#ifdef DEBUG 

int stack_dump_(struct Stack* stack, LOG_PARAMS) {

    stack_log_report();
    STACK_PTR_CHECK(stack);

    fprintf(logs_file, "\n <pre> <div class = \"outline\" style = \"background-color:lightgrey; style = \"text_align: center;>\n");

    fprintf(logs_file, "\n""Critical error occured"
                       " during work with stack.""\n");

    fprintf(logs_file, "\n""Function srack_dump has"
                       " been called from %s, %s line %d""\n",
                         ___func_name, ___file_name, ___line);

    fprintf(logs_file, "Stack address <%p>. Stack:"
                       " element type %s""\n\n", stack, TYPE_NAME);

    fprintf(logs_file, "Stack origin conctruction file name: ");

    if (stack->origin.orig_file_name == NULL)
        fprintf(logs_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_file_name != NULL
        && stack_empty_orig_file_check(stack, LOGS_ARGS_USE) == 1)

        fprintf(logs_file, "ERROR: EMPTY ORIG FILE NAME\n");

    else
        fprintf(logs_file, "%s\n", stack->origin.orig_file_name);


    fprintf(logs_file, "Stack origin conctruction function name: ");

    if (stack->origin.orig_func_name == NULL)
        fprintf(logs_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_func_name != NULL
        && stack_empty_orig_func_check(stack, LOGS_ARGS_USE) == 1)

        fprintf(logs_file, "ERROR: EMPTY ORIG FUNCTION NAME\n");

    else 

        fprintf(logs_file, "%s\n", stack->origin.orig_func_name);


    fprintf(logs_file, "Stack origin construction line: ");

    if (stack->origin.orig_line == 0)
        fprintf(logs_file, "ERROR: Line number equals zero\n");
    else
        fprintf(logs_file, "%d\n", stack->origin.orig_line);


    fprintf(logs_file, "Size of element type is %lu\n", sizeof(elem_t));

    if (stack->size_of_elem != sizeof(elem_t))
        fprintf(logs_file, "ERROR: Size of element type is not equal"
                                         " size of stack element\n");

    if (stack->size_of_elem == 0)
        fprintf(logs_file, "ERROR: Size of element equals zero\n");

    if (stack->size_of_elem != 0)
        fprintf(logs_file, "Size of stack element: %d\n", 
                                    stack->size_of_elem);

    #ifdef CANARIES

        fprintf(logs_file, "Canary protection: ");

        if (stack->canary1 == Canary_val)
            fprintf(logs_file, "left is valid, ");
        else
            fprintf(logs_file, "ERROR left canary protection failed, ");

        if (stack->canary2 == Canary_val)
            fprintf(logs_file, "right is valid\n\n");
        else
            fprintf(logs_file, "ERROR right canary protection failed\n\n");

    #endif

    fprintf(logs_file, "Stack capacity : %d. Number of"
                            " elements in stack: %d\n",
                        stack->capacity, stack->count);

    if (stack->capacity == 0)
        fprintf(logs_file, "ERROR: Capacity equals zero\n");

    if (stack->capacity < 0)
        fprintf(logs_file, "ERROR: Capacity is negative value\n");

    if (stack->count < 0)
        fprintf(logs_file, "ERROR: Number of elements in"
                            " stack is negative value\n");

    if (stack->count > stack->capacity)
        fprintf(logs_file, "ERROR: Capacity cannot be lower than number"
                                             " of elements in stack\n");
    #ifdef CANARIES

        int64_t* canary_ptr1 = (int64_t*)stack->data - 1;
        int64_t* canary_ptr2 = (int64_t*)((char*)stack->data 
                                        + (unsigned long)stack->capacity * sizeof(elem_t));
    
        if (stack->capacity >= 0
            && stack->data != NULL
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != (elem_t*)DEFAULT_PTR 
            && *canary_ptr1 != Canary_val)

            fprintf(logs_file, "ERROR: left canary protection "
                               "of stack->data has fallen.\n");

        else if (stack->capacity >= 0
            && stack->data != NULL
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != (elem_t*)DEFAULT_PTR
            && *canary_ptr2 != Canary_val)

            fprintf(logs_file, "ERROR: right canary protection of"
                                    " stack->data has fallen.\n");

        else
            fprintf(logs_file, "Canary protection works\n");

    #endif

    fprintf(logs_file, "Stack data pointer: ");

    if (stack->data == NULL)
        fprintf(logs_file, "ERROR: NULL data pointer\n\n");

    else if (stack->data == (elem_t*)DEFAULT_PTR && stack->capacity != 0)

        fprintf(logs_file, "ERROR: Data pointer is incorrect.\n "
                               "Can not equal DEFAULT_PTR after "
                                      "pushing or size setting");

    else if (stack->data == (elem_t*)POISON_PTR)
        fprintf(logs_file, "ERROR: POISON_PTR for stack->data"
                           " means that stack was destructed");

    else if (stack->data == (elem_t*)DEFAULT_PTR 
          && stack->count == 0)

        fprintf(logs_file, "Data pointer is DEFAUlT_PTR "
                            "Probably stack has not been"
                                " used to push elements");

    else fprintf(logs_file, "<%p>\n", stack->data);

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->count > 0 
        && stack->capacity > 0 
        && stack->capacity >= stack->count
        && clear_mem_check(&stack->data[stack->count], 
                            stack->capacity - stack->count + 1,
                            stack->size_of_elem)) {

        fprintf(logs_file, "Elements of the stack: \n\n");

        for (int count = 0; count < stack->count; count++) {

            fprintf(logs_file, "[%d]" ELEM_SPEC "\n", 
                      count, stack->data[count - 1]);
        }
    }

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && !clear_mem_check(&stack->data[stack->count],
                             stack->capacity - stack->count + 1, 
                             stack->size_of_elem))

        fprintf(logs_file, "ERROR: Stack free data is not empty\n");

    if (stack->count == 0 && stack->capacity == 0)
        fprintf(logs_file, "Stack is empty\n");

    stack_free_data(stack);

    fprintf(logs_file, "\n </div> </pre>\n");

    fflush(logs_file);

    assert(0 && "CRITICAL ERROR: Stack dump has been called");
    return 0;
}
#endif


