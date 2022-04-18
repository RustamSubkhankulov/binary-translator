#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "text_processing.h"
#include "../logs/errors_and_logs.h"

//#define PRINTNUMBEROFLINE

//=============================================================================

// int my_swap(void* first_, void* second_, size_t size) {

// 	assert(first_  != NULL);
// 	assert(second_ != NULL);

// 	char* first_ptr  = (char*)first_;
// 	char* second_ptr = (char*)second_;

// 	while (size >= sizeof(int64_t)) {

// 		int64_t temp = *(int64_t*)first_ptr;
// 		*(int64_t*)first_ptr  = *(int64_t*)second_ptr;
// 		*(int64_t*)second_ptr = temp;

// 		first_ptr += sizeof(int64_t);
// 		second_ptr += sizeof(int64_t);

// 		size -= sizeof(int64_t);
// 	}

// 	while (size >= sizeof(int32_t)) {

// 		int32_t temp = *(int32_t*)first_ptr;
// 		*(int32_t*)first_ptr = *(int32_t*)second_ptr;
// 		*(int32_t*)second_ptr = temp;

// 		first_ptr += sizeof(int32_t);
// 		second_ptr += sizeof(int32_t);

// 		size -= sizeof(int32_t);
// 	}

// 	while (size >= sizeof(int16_t)) {

// 		int16_t temp = *(int16_t*)first_ptr;
// 		*(int16_t*)first_ptr  = *(int16_t*)second_ptr;
// 		*(int16_t*)second_ptr = temp;

// 		first_ptr += sizeof(int16_t);
// 		second_ptr += sizeof(int16_t);

// 		size -= sizeof(int16_t);
// 	}

// 	while (size >= sizeof(int8_t)) {

// 		int8_t temp = *(int8_t*)first_ptr;
// 		*(int8_t*)first_ptr  = *(int8_t*)second_ptr;
// 		*(int8_t*)second_ptr = temp;

// 		first_ptr += sizeof(int8_t);
// 		second_ptr += sizeof(int8_t);

// 		size -= sizeof(int8_t);
// 	}

// 	return 0;
// }

//=============================================================================
char* text_init(const char* filename, struct Text* text, LOG_PARAMS) {

	text_log_report();

	//filename = "hamlet.txt";

	assert(filename != NULL);
	assert(text != NULL);

	errno = 0;

	char* buf = file_to_buf_copy(filename, text, LOG_ARGS);
	if (buf == NULL)
		return NULL;

	text->buf = buf;

	strings_init(buf, text);

	#ifdef LOGS
		fprintf(log_file, "Number of strings read is"
					" %ld\n Size of the file: %ld.\n",
								 text->strings_number,
		     							  text->size);
	#endif

	return buf;
}

//============================================================================

char* _copy_from_file_to_buffer(const char* filename, int* size_ptr, LOG_PARAMS) {

	text_log_report();

	if (filename == NULL) {

		error_report(INV_FILE_NAME);
		return NULL;
	}

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {

		error_report(FOPEN_ERROR);
		return NULL;
	}
	
	long size = file_size_(fp);
	if (size < -1)
		return NULL;

	char* buf = copy_data_to_buf(size, fp, LOG_ARGS);

	fclose(fp);

	#ifdef LOGS
		fprintf(log_file, "File %s copied to buffer. Size of copied data is %ld.\n\n",
																		    filename,
																		 text->size);
	#endif

	int ret = replace_nulls_with_spaces(buf, size);
	if (ret == -1)
		return NULL;

	*size_ptr = (int)size;
	return buf;
}

//============================================================================

char* file_to_buf_copy(const char* filename, struct Text* text, LOG_PARAMS) {

	text_log_report();

	assert(filename != NULL);
	assert(text != NULL);

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {

		error_report(FOPEN_ERROR);
		return NULL;
	}
	
	text->size = file_size_(fp);
	if (text->size < -1)
		return NULL;

	char* buf = copy_data_to_buf(text->size, fp, LOG_ARGS);

	fclose(fp);

	#ifdef LOGS
		fprintf(log_file, "File %s copied to buffer. Size of copied data is %ld.\n\n",
																		    filename,
																		 text->size);
	#endif

	return buf;
}

//=============================================================================
long file_size(FILE* fp, LOG_PARAMS) {

	text_log_report();

	assert(fp != NULL);

	int fseek_return = fseek(fp, 0L, SEEK_END);
	if (fseek_return != 0) 
		return -1;

	long size = ftell(fp);
	if (size == -1L) 
		return -1;

	rewind(fp);

	return size;
}

//============================================================================
char* copy_data_to_buf(long size, FILE* fp, LOG_PARAMS) {

	text_log_report();

	assert(fp != NULL);

	char* buf = (char*)calloc((size_t)(size + 1), sizeof(char));
	if   (buf == NULL) {

		error_report(CANNOT_ALLOCATE_MEM);
		return NULL;
	}

	long end_of_file = (long)fread(buf, sizeof(char), 
								   (size_t)size, fp);
	if  (end_of_file != size) {
		
		error_report(FREAD_ERR);
		return NULL;
	}

	buf[end_of_file] = '\0';

	return buf;
}

//============================================================================
long count_strings(char* buf) {

	assert(buf != NULL);

	char* n_pointer = buf;
	long strings_number = 0;

	while ((n_pointer = strchr(n_pointer, '\n')) != NULL) {

		strings_number++;
		n_pointer++;
	}

	#ifdef LOGS
		fprintf(log_file, "Strings counted. Number of strings %ld.\n", strings_number);
	#endif

	return strings_number;
}

//============================================================================
void strings_init(char* buf, struct Text* text) {

	assert(buf != NULL);
	assert(text != NULL);

	text->strings_number = count_strings(buf);

	text->strings = (struct String*)calloc((size_t)text->strings_number,
										 	     sizeof(struct String));

	assert(text->strings != NULL);

	struct String* starting_pointer = text->strings;

	char* string_start = strtok(buf, "\n");
	assert(string_start != NULL);

	text->strings_number = 0;
	long length = 0;

	while (string_start != NULL) {
		string_null_carriage_return(string_start);
		
		if (does_contain_letters(string_start)) { 

			string_start = string_skip_blank(string_start);
			string_null_carriage_return(string_start);
			length = (long)strlen(string_start);
			text->strings_number++;

			string_init(text->strings, length, string_start, text->strings_number);

			text->strings++;
		}
		string_start = strtok(NULL, "\n");
	}

	text->strings = starting_pointer;

	#ifdef LOGS
		fprintf(log_file, "Strings (expept empty strings) initializated."
								 " Number of initializated strings %ld\n",
													text->strings_number);
	#endif


}

//===========================================================================
int does_contain_letters(const char* string) {

	assert(string != NULL);

	const char* string_ptr = string;
	while (*string_ptr != '\0') {

		if (!isblank(*string_ptr)) return 1;
		string_ptr++;
	}

	return 0;
}

//============================================================================
char* string_skip_blank(char* string) {

	assert(string != NULL);

	while (*string == ' ') 
		    string++;

	return string;
}

//============================================================================

int replace_nulls_with_spaces(char* buffer, long size) {

	assert(buffer);

	for (long counter = 0; counter < size; counter++) {

		if (buffer[counter] == 0)
			buffer[counter] = 32;
	}

	buffer[size] = '\0';

	return 0;
}

//============================================================================

int string_prepare(char* string, int size) {

	assert(string);

	for (int counter = 0; counter < size; counter++) {

		if (string[counter] == '\r' || string[counter] == '\n')
			string[counter] = 32;
	}

	return 0;
}

//============================================================================

int string_null_carriage_return(char* string) {

	assert(string != NULL);

	int counter = 0;

	while(string[counter] != '\0') {

		if (string[counter] == '\r')
			string[counter] = '\0';
		counter++;
	}

	return 0;
}

//============================================================================
void string_init(struct String* strings, long length, char* data, long number) {

	assert(strings != NULL);
	assert(data != NULL);

	strings->length = length;
	strings->data   = data;
	strings->number = number;
}

//==============================================================================================
void clear_strings(struct Text* text) {

	assert(text != NULL);

	if (text->strings_number != 0)
		free(text->strings);

	text->strings_number = 0;

	#ifdef LOGS
		fprintf(log_file, "Memory allocated for strings is free.\n");
	#endif
}

//===========================================================================================

struct Text* print_strings(struct Text* text, const char* filename) {

	assert(text != NULL);
	assert(filename != NULL);

	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) 
		return NULL;

	for (long string_count = 0; string_count < text->strings_number; string_count++) {


		#ifdef PRINTNUMBEROFLINE
			fprintf(fp, "%5ld %5ld", text->strings[string_count].number, \
									text->strings[string_count].length);
		#endif

		fprintf(fp, " %s\n", text->strings[string_count].data);

	}

	int fclose_return =  fclose(fp);
	if (fclose_return != 0)
		return NULL;
	
	return text;
}

//============================================================================================

void clear_buf(char* buf) {
	
	assert(buf != NULL);

	free(buf);
}


//==============================================================================================

int text_clear_mem_(struct Text* text, char* buf, LOG_PARAMS) {

	text_log_report();

	if (text == NULL) {

		error_report(INV_TEXT_PTR);
		return -1;
	}
	
	if (buf == NULL) {

		error_report(INV_BUF_PTR);
		return -1;
	}

	clear_buf(buf);
	clear_strings(text);

	return 0;
}

//==============================================================================================

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                            int size, int pos, LOG_PARAMS)  {

    text_log_report();
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

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    text_log_report();
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

int _buffer_free(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

	text_log_report();

	free(buffer_struct->buffer);
	return 0;
}