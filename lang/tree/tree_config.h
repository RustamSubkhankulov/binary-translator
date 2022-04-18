#pragma once

#include <cstddef>

#include "../diff/diff_config.h"

//===================================================================

 const int Dir_name_size = 100;

//===================================================================

//TREE OPTIONS

#define TREE_VERIFICATE

//Logs for tree
//Generates .png files in /images and list_logs.html in text_files/
//#define TREE_LOGS

//Calls dump of the tree every time validator is called
//#define TREE_DEBUG

//GRAPHVIZ
#define TREE_GRAPHVIZ

//Definition for hash calculating
#define HASH_POINTER_TO_CHAR

//==================================================================

//SUPPORTING DEFINITIONS

const unsigned Poison_value = 228;

//==================================================================