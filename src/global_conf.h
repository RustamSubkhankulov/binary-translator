#pragma once 

//===============================================

#define LOG_DIR "/tmp/"

//#define LOGS

//-----------------------------------------------

// Places 'int 3' interrupt just before executing translated code

//#define DEBUG_EXEC

//-----------------------------------------------

// turns on binary optimizations

#define BINARY_OPT

//-----------------------------------------------

// removes nops from translated code 

//#define REMOVE_NOPS

//===============================================

static const char* Logfile_name = "trans.html";

//===============================================

// Turns on file input / output for jit

//#define FILE_IO

#ifdef  FILE_IO

    static const char* Std_input_filename  = "text_io/input.txt";

    static const char* Std_output_filename = "text_io/output.txt";

#endif 

//===============================================

//#define ENTITY_ADD_NAME_STR

//===============================================