#pragma once

#include <stdio.h>

#include "../lang_common/lang_config.h"
#include "../logs/errors_and_logs.h"

//===================================================================

//Configuration
#define TEXT_LOGS

//===================================================================

struct Buffer_struct {

    char* buffer;

    int size;
    int pos;
};

//===================================================================

#define BUFFER_STRUCT_PTR_CHECK(buffer_struct_ptr) {                \
                                                                    \
    if (buffer_struct_ptr == NULL) {                                \
                                                                    \
        error_report(BUFFER_STRUCT_INV_PTR);                        \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

//#define TEMP_DIR "/tmp/"

//===================================================================

#ifdef TEXT_LOGS

	#define text_log_report() \
			log_report()

#else

	#define text_log_report() ""

#endif

//===================================================================

#define buffer_struct_init(buffer_struct, buffer, size, pos) \
       _buffer_struct_init(buffer_struct, buffer, size, pos, LOG_ARGS)

#define buffer_dump(buffer_struct) \
       _buffer_dump(buffer_struct, LOG_ARGS)

#define buffer_free(buffer_struct) \
       _buffer_free(buffer_struct, LOG_ARGS)

#define text_init_(filename, text) \
		text_init(filename, text, LOG_ARGS)

#define file_size_(fp) \
	    file_size(fp, LOG_ARGS)

#define text_clear_mem(text, buf) \
		text_clear_mem_(text, buf, LOG_ARGS);

#define copy_from_file_to_buffer(filename, size_ptr) \
	   _copy_from_file_to_buffer(filename, size_ptr, LOG_ARGS)

//===================================================================

/// Structure contains string as char* and its length
struct String {

	char* data;
	long length;
	long number;
};

/// Structure contains text read from file
/// 
/// Structure contains array of struct String, number of strings and size of text
struct Text {

	long strings_number;
	struct String* strings;
	long size;

	char* buf;
};

/// Function initializates text
/// 
/// Function reads file to structure Text 
/// Clears allocated memoru after strings initialization
/// @param filename - name of the file
/// @param struct Text* text pointer to structure Text
char* text_init(const char* filename, struct Text* text, LOG_PARAMS);

/// Reads size of the file
///
/// <param name="fp">pointer to the file</param>
/// <returns>Size of file in bytes</returns
long file_size(FILE* fp, LOG_PARAMS);

/// Counts strings in file
///
/// @param char* buf - buffer that contains text as string
/// @returns number of the strings
long count_strings(char* buf);

/// Copies strings one by one from char* buf
/// 
/// @param char* buf pointer to the buffer
/// @param struct Text* text pointer to the text structure
void strings_init(char* buf, struct Text* text);

/// Copies data from text file to char* buf buffer
/// 
/// Opens file, call copy_to_buf function ans closes file.
/// @param const char* filename name of the file that will be copied to char* buf
/// @param struct Text* text pointer to the Text structure
/// @returns char* pointer to the buffer
char* file_to_buf_copy(const char* filename, struct Text* text, LOG_PARAMS);

/// Checks if there any letters in string
/// 
/// @param const char* pointer to string
/// @returns 1, 0 ot -1 if first string lexicographically more, equal or less than second one
int does_contain_letters(const char* string);

/// String initialization with arguements of function
/// 
/// @param struct String* strings poinet to the one of String structures
/// @param length of the strings
/// @param char* data pointer to the string located in buf
/// @param number of the string
void string_init(struct String* strings, long length, char* data, long number);

//Skips all non_letter characters an the start of each string
///
///@param char* string pointer to the string 
///@returns char* pointer to the first literal character
char* string_skip_blank(char* string);

///Copies data fron file to buffer using fread
///
///allocates memory for data from file
///@warning requires free function to clear buffer
///@param long size size of file if bytes
///@param FILE* fp pointer to the file
char* copy_data_to_buf(long size, FILE* fp, LOG_PARAMS);

/// Prints strings
/// 
/// prints number of string, its length and string itself
/// @param struct Text* text - pointer to the text structure
/// @return struct Text* text pointer of NULL ptr if error occures
struct Text* print_strings(struct Text* text, const char* filename);

/// Clears allocated for strings memory
/// 
/// uses free to clear allocated memory
/// @param oiunter to the structure
void clear_strings(struct Text* text);

///Clears buffer
///
/// Clears memory allocated for text data
/// @param char * pointer to the buffer
void clear_buf(char* buf);

///Clears allocated memory
///
/// Calls clear_strings and clear_buf
/// @param struct Text* text pointer to text structure
/// @param char* buf pointer to the buffer
int text_clear_mem_(struct Text* text, char* buf, LOG_PARAMS);

//===================================================================

int string_null_carriage_return(char* string);

//===================================================================

int replace_nulls_with_spaces(char* buffer, long size);

//===================================================================

char* _copy_from_file_to_buffer(const char* filename, int* size, LOG_PARAMS);

//===================================================================

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS);

//===================================================================

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                           int size, int pos, LOG_PARAMS);

//===================================================================

int _buffer_free(struct Buffer_struct* buffer_struct, LOG_PARAMS);

//===================================================================

int string_prepare(char* string, int size);
