#pragma once

#include <cstddef>
#include "../global_conf.h"

//===================================================================

#include "../global_conf.h"
#include "../bintrans/bintrans.h"

//===================================================================

#ifdef LOGS

    //#define LIST_LOGS

#endif 

//===================================================================

#define SYSTEM_COMMAND_BUF_SIZE 200

//===================================================================

//LIST PARAMETERS

//Element type of the list structure
typedef struct Instr* list_elem_t;

//Name of type using in list(for logs)
#define LIST_TYPE_NAME "struct Instr*"

//Specificator for type using in list
#define LIST_ELEM_SPEC "%p"

//Starting list capacity
const size_t List_start_capacity = 16;

//Max list capacity
const size_t List_max_capacity = 262144;

//===================================================================

//LIST OPTIONS

#ifdef LIST_LOGS

    //Calls dump of the list every time validator is called
    #define LIST_DUMP

    //GRAPHVIZ usage for dumps
    #define LIST_GRAPHVIZ

#endif 

//Hash protection for list
#define LIST_HASH

//Control pointers to allocated memory
#define LIST_PTR_CNTRL

// Remove all log report, leave only dumps 
//#define LIST_ONLY_DUMPS

// Turns on list validator check 
//#define LIST_VALID_CHECK

// Turns of all additionals checks in ctor, dtor and etc
#define LIST_FAST

//==================================================================

//SUPPORTING DEFINITIONS

#define LIST_POISON_VALUE 228

//==================================================================
