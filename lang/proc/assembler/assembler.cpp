#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "../../general/general.h"
#include "assembler.h"

//===================================================================

static FILE* Asm_listing_file = NULL;

//===================================================================

static char operation_name_buf[OPERATION_NAME_BUF_SIZE] = { 0 };

//===================================================================

int _print_incorrect_string(const char* string, int str_number, 
												  LOG_PARAMS) {

	asm_log_report();

	fprintf(logs_file, "Incorrect input. String number %d. " 			
					     "String: %s.", str_number, string);

	return 0;				
}

//===================================================================

int text_remove_comments_(struct Text* text, LOG_PARAMS) {

	asm_log_report();

	if (text == NULL)
		error_report(INV_TEXT_PTR);

	for (int str_count = 0; str_count < text->strings_number; str_count++) {

		char* ptr = strchr(text->strings[str_count].data, ';');

		if (ptr != NULL) {
		  
		   *ptr = '\0';

		   text->strings[str_count].length = 
		   				 (long int) strlen(text->strings[str_count].data);
		}	
	}
	
	return 0;
}

//===========================================================================

#undef DEF_CMD_

#define DEF_CMD_(num, name, op_code, hash, instructions)					\
																			\
	if (oper_hash == hash) {				    							\
																			\
		long code_ip_position = asmstruct->ip - asmstruct->code;  			\
		asmstruct->ip++;													\
		ASMSTRUCT_IP_CHECK(asmstruct);										\
																			\
		asm_string.oper_code = op_code;										\
																			\
		if (num != 0) {														\
																			\
			int ret = check_is_ram_using_command(&asm_string);				\
			if (ret == -1)													\
				return -1;													\
																			\
			for (int ct = 0; ct < num; ct++) {								\
				ret = read_argument(&asm_string, asmstruct);				\
																			\
				if (ret == -1)												\
					return -1;												\
			}																\
																			\
			check_second_square_bracket(&asm_string);						\
		}	                                                                \
																			\
		*(asmstruct->code + code_ip_position) = asm_string.oper_code;		\
	}																		\
	else

//===========================================================================

#define DEF_JMP_(num, name, op_code, hash, instructions)		        	\
																			\
	if (oper_hash == hash) {												\
																			\
		*asmstruct->ip = op_code;											\
																			\
		asmstruct->ip++;													\
		ASMSTRUCT_CHECK(asmstruct);											\
																			\
		const char* ptr = strchr(string + byte_end - 1, ':');				\
																			\
		if (ptr != NULL)													\
			read_label_name(asmstruct, &asm_string, ptr);					\
		else																\
			read_imm_label(asmstruct, &asm_string);							\
	}																		\
	else 

//===========================================================================

int process_string_(const char* string, struct Asmstruct* asmstruct, 
									     int str_number, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);
	CHAR_PTR_CHECK(string);
	
	save_string_start_ip(asmstruct, str_number);

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];

	int byte_ct    = 0;
	int byte_start = 0;
	int byte_end   = 0;

	int scan_cmnd = 
		sscanf(string, " %n%s%n %n", &byte_start, operation_name_buf, &byte_end, &byte_ct);

	if (scan_cmnd == 0) {

		print_incorrect_string(string, str_number);
		error_report(COMPILATION_ERR);
	}

	unsigned int length = (unsigned)(byte_end - byte_start);
	label_name_fix(operation_name_buf, &length);

	int64_t oper_hash = get_hash(operation_name_buf, length);

	struct Asm_string asm_string = { 0 };

	asm_string.string = string;
	asm_string.str_number = str_number;
	asm_string.byte_ct = byte_ct;
							
	#include "../text_files/commands.txt"
	#include "../text_files/jumps.txt"
	//else 
	{
		print_incorrect_string(string, str_number);
		error_report(COMPILATION_ERR);
	};

	#undef DEF_CMD_
	#undef DEF_JMP_

	clear_opernamebuf();

	//CHECK_IF_STRING_IS_EMPTY(string + asm_string.byte_ct);

	#ifdef LISTING

		write_listing(string, length, asmstruct, str_number);

	#endif

	return 0;
}

//==================================================================


int _jumps_fill_gaps(struct Asmstruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	for (int i = 0; i < asmstruct->ud_jumps_number; i++) {

		int jump_pos = asmstruct->ud_jumps[i].code_pos;

		int dest_pos = search_label_name(asmstruct->ud_jumps[i].label_hash,
										                        asmstruct);

		if (dest_pos == -1)
			return -1;

		if (dest_pos == -2)
			error_report(NO_LABEL_FOR_JUMP);

		*(int*)(asmstruct->code + jump_pos) = dest_pos;

		#ifdef LISTING

			fix_up_listing(jump_pos, dest_pos);

		#endif
	}

	for (int i = 0; i < asmstruct->str_jumps_number; i++) {

		if (asmstruct->str_jumps[i].string_number < 1
		||	asmstruct->str_jumps[i].string_number > asmstruct->strings_number)

		error_report(INV_STRING_NUMBER);

		int dest_pos = get_string_start_ip(asmstruct->str_jumps[i].string_number, 
										 							 asmstruct);
		if (dest_pos == -1)
			return -1;

		int jump_pos = asmstruct->str_jumps[i].code_ip;

		*(int*)(asmstruct->code + jump_pos) = dest_pos;

		#ifdef LISTING

			fix_up_listing(jump_pos, dest_pos);

		#endif
	}
	
	return 0;
}

//==================================================================

int _get_string_start_ip(int str_number, struct Asmstruct* asmstruct, 
												          LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	if (str_number == 1)
		return sizeof(struct Header);
	
	if (str_number < 1 || str_number > asmstruct->strings_number)
		error_report(INV_STRING_NUMBER);

	int code_pos = asmstruct->string_start_ips[str_number - 1];

	if (code_pos == 0)
		code_pos = get_string_start_ip(str_number + 1, asmstruct);

	return code_pos;
}

//==================================================================

int _add_ud_label(const char* string, int ip, struct Asmstruct* asmstruct, 
                                                               LOG_PARAMS) {

	asm_log_report();

	CHAR_PTR_CHECK(string);

	if (asmstruct->ud_jumps_capacity == 0) {

		asmstruct->ud_jumps = (Label*)calloc(UD_JUMPS_NUMBER,
										      sizeof(Label));
		asmstruct->ud_jumps_capacity = UD_JUMPS_NUMBER;											
	}

	else if (asmstruct->ud_jumps_number >= asmstruct->ud_jumps_capacity) {

		asmstruct->ud_jumps = (Label*)my_recalloc(asmstruct->ud_jumps,
						 						 asmstruct->ud_jumps_capacity * 2,
												 asmstruct->ud_jumps_capacity,
												 sizeof(Label));
		asmstruct->ud_jumps_capacity *= 2;
	}

	if (asmstruct->ud_jumps == NULL)
		return -1;

	int number = asmstruct->ud_jumps_number;

	int length = (int)strlen(string);
	int64_t ud_label_hash = get_hash((void*)string, (unsigned)length);

	asmstruct->ud_jumps[number].label_hash = ud_label_hash;
	asmstruct->ud_jumps[number].code_pos = ip;

	asmstruct->ud_jumps_number++;

	return 0;
}

//==================================================================

int _search_label_name(int64_t label_hash, struct Asmstruct* asmstruct, 
										                    LOG_PARAMS) {

	asm_log_report();
    ASMSTRUCT_CHECK(asmstruct);

	if (asmstruct->labels_number == 0)
		return -2;

	int cntr = 0;
	while (cntr < asmstruct->labels_number) {

		if (label_hash == asmstruct->labels[cntr].label_hash)
		break;

		cntr++;
	}

	if (cntr == asmstruct->labels_number)
		return -2;

	else 
		return asmstruct->labels[cntr].code_pos;
	
}

//==================================================================

int clear_opernamebuf_(LOG_PARAMS) {

	asm_log_report();

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];

	for (int ct = 0; ct < OPERATION_NAME_BUF_SIZE; ct++)
		operation_name_buf[ct] = 0;

	return 0;
}

//===================================================================

#define DEF_JMP_(num, name, code, hash, instructions)		        \
																	\
	case hash: {													\
																	\
		return 1;													\
	}

//===================================================================

int _scan_label(const char* string, struct Label_temp* label_temp, 
													   LOG_PARAMS) {
	asm_log_report();

	int byte_start = 0;
	int byte_end = 0;

	char label_name_buf[LABEL_NAME_SIZE] = { 0 };

		int sscanf_ret = sscanf(string, " %n%s%n", &byte_start, 			
												label_name_buf, 			
													&byte_end);			
		
		int label_name_length = byte_end - byte_start;							
																			
		if (sscanf_ret < 1) {												
																			
			print_incorrect_string(string, label_temp->str_number);											
			error_report(COMPILATION_ERR);							
		}																
																			
		char symbol = 0;													
		int byte_offset = 0;												
																			
		sscanf_ret = sscanf(string + byte_end, " %c%n", 					
							  	&symbol, &byte_offset);						
		if (sscanf_ret == 1)											
		byte_end += byte_offset;
																	
		if ((label_name_buf[label_name_length - 1] != ':' 					
			&& sscanf_ret != 1)												
		 	|| (sscanf_ret == 1 && symbol != ':'))							
																			
			return STRING_IS_NOT_LABEL;														
																			
		if (label_name_length  > LABEL_NAME_SIZE) {							
																			
			print_incorrect_string(string, label_temp->str_number);											
			error_report(LABEL_NAME_TOO_THICK);						
		}																	
																		
		if (label_name_buf[label_name_length - 1] == ':') {					
																			
			label_name_buf[label_name_length - 1] = '\0';					
			label_name_length -= 1;											
		}

		label_temp->label_name_length = label_name_length;
		strncpy(label_temp->label_name, label_name_buf, LABEL_NAME_SIZE);

		return byte_end;
}

//===================================================================

int read_label_(const char* string, struct Asmstruct* asmstruct, 
									 int str_number, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);
	CHAR_PTR_CHECK(string);

	char label_name_buf[LABEL_NAME_SIZE] = { 0 };

	struct Label_temp label_temp = { 0 };
	label_temp.str_number = str_number;
	label_temp.label_name = label_name_buf;

	int byte_end = scan_label(string, &label_temp);

	if (byte_end == STRING_IS_NOT_LABEL)
		return STRING_IS_NOT_LABEL;

	if (byte_end == -1)
		return -1;

	int64_t label_hash = get_hash(label_temp.label_name, 
								  (unsigned)label_temp.label_name_length);
	switch (label_hash) {

		#include "../text_files/jumps.txt"
		default:; break;
	}
	#undef DEF_JMP_

	int do_label_repeat = search_label_name(label_hash, asmstruct);
	
	if (do_label_repeat == -1) 
		return -1;
	
	if (do_label_repeat != -2) 
		error_report(REPEATING_LABELS)
	
	else {
		
		int ret = add_label(asmstruct, label_hash);
		if (ret == -1)
			error_report(CANNOT_ADD_NEW_LABEL);
	}

	CHECK_IF_STRING_IS_EMPTY(string + byte_end);

	return 0;
}

//===================================================================

int _save_string_start_ip(struct Asmstruct* asmstruct, int number, 
								                       LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	int ip = (int)(asmstruct->ip - asmstruct->code);

	if (ip > ASM_MAX_CODE_SIZE)
		error_report(INV_INSTR_PTR);

	if (number > asmstruct->strings_number)
		error_report(INV_STRING_NUMBER);

	asmstruct->string_start_ips[number] = ip;

	return 0;
}

//===================================================================

int _add_string_jump(int code_ip, int dest_string, struct Asmstruct* asmstruct, 
												                    LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	if (asmstruct->str_jumps_capacity == 0) {

		asmstruct->str_jumps = (struct Str_jump*)calloc(STRING_JUMPS_NUMBER, 
						   				           sizeof(Str_jump));
		asmstruct->str_jumps_capacity = STRING_JUMPS_NUMBER;
	}

	else if (asmstruct->str_jumps_number >= asmstruct->str_jumps_capacity) {

		asmstruct->str_jumps = (Str_jump*)my_recalloc(asmstruct->str_jumps, 
													  asmstruct->str_jumps_capacity * 2, 
											  		  asmstruct->str_jumps_capacity, 
													  sizeof(struct Str_jump));

		asmstruct->str_jumps_capacity *= 2;
	}

	if (asmstruct->str_jumps == NULL)
		return -1;

	int number = asmstruct->str_jumps_number;

	asmstruct->str_jumps[number].code_ip = code_ip;
	asmstruct->str_jumps[number].string_number = dest_string;

	asmstruct->str_jumps_number++;

	return 0;
}

//===================================================================

int _add_label(struct Asmstruct* asmstruct, int64_t label_hash, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);
	
	if (asmstruct->labels_capacity == 0) {

		asmstruct->labels = (Label*)calloc(START_LABELS_NUMBER, 
										 sizeof(struct Label));
		asmstruct->labels_capacity = START_LABELS_NUMBER;
	}

	else if (asmstruct->labels_number  >= asmstruct->labels_capacity) {

		asmstruct->labels = (Label*)my_recalloc(asmstruct->labels, 
											    asmstruct->labels_capacity * 2, 
											    asmstruct->labels_capacity, 
											    sizeof(struct Label));

		asmstruct->labels_capacity *= 2;
	}

	if (asmstruct->labels == NULL)
		return -1;

	int label_num = asmstruct->labels_number;

	asmstruct->labels[label_num].code_pos= (int)(asmstruct->ip - asmstruct->code);
	asmstruct->labels[label_num].label_hash = label_hash;
	
	asmstruct->labels_number++;

	return 0;
}

//===================================================================

void print_labels(struct Asmstruct* asmstruct) {

	for (int i = 0; i < asmstruct->labels_number; i++) {

		printf("\n\n %ld %x \n\n", asmstruct->labels[i].label_hash,                                  
						 (unsigned)asmstruct->labels[i].code_pos);                                    
	}
}

//===================================================================

int convert_operations_to_binary_(struct Text* text, struct Asmstruct* asmstruct,
													        	      LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_PTR_CHECK(asmstruct);
	TEXT_PTR_CHECK(text);

	if (text->strings_number <= 0)
		error_report(TEXT_NO_STRINGS)

	else {

		int ret = asmstruct_allocate_memory(text->strings_number, asmstruct);
		if (ret == -1)
			return -1;	

		asmstruct->strings_number = (int)text->strings_number;
	}

	asmstruct->ip = asmstruct->code + HDRSIZE;

	for (int str_count = 0; 
			 str_count < text->strings_number; 
			 str_count++) {

		if (text->strings[str_count].data == NULL
			|| !does_contain_letters(text->strings[str_count].data))
			continue;

		int value = read_label(text->strings[str_count].data, 
									   asmstruct, str_count);

		if (value == -1)
			return -1;
		if (value == 0)
			continue;

		value = process_string(text->strings[str_count].data, 
											       asmstruct, 
												  str_count);
		if (value == -1) 
			return -1;
	}

	jumps_fill_gaps(asmstruct);

	return 0;
}

//===================================================================

int _asmstruct_free_memory(struct Asmstruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	if (asmstruct->code != NULL)
		free(asmstruct->code);

	if (asmstruct->labels != NULL)
		free(asmstruct->labels);

	if (asmstruct->ud_jumps != NULL)
		free(asmstruct->ud_jumps);

	if (asmstruct->str_jumps != NULL)
		free(asmstruct->str_jumps);

	if (asmstruct->string_start_ips != NULL)
		free(asmstruct->string_start_ips);

	return 0;
}

//===================================================================

int _asmstruct_allocate_memory(long number, struct Asmstruct* asmstruct, 
										                     LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_PTR_CHECK(asmstruct);

	void* ptr = calloc(ASM_MAX_CODE_SIZE, sizeof(char));

	if (ptr == NULL)
		error_report(CANNOT_ALLOCATE_MEM)
	else
		asmstruct->code = (unsigned char*)ptr;

	ptr = calloc((unsigned)number, sizeof(int));

	if (ptr == NULL)
		error_report(CANNOT_ALLOCATE_MEM)
	else
		asmstruct->string_start_ips = (int*)ptr;
	
	return 0;
}

//===================================================================

FILE* open_code_file_(const char* filename, const char* mode, 
								       			  LOG_PARAMS) {

	asm_log_report();

	if (filename == NULL) {

		error_report(INV_FILE_NAME);
		return NULL;
	}

	if (mode == NULL) {

		error_report(NULL_STRING);
		return NULL;
	}

	FILE* fp = fopen(filename, mode);

	if (fp == NULL) {

		error_report(FOPEN_ERROR);
		return NULL;
	}

	return fp;
}

//===================================================================

int close_code_file_(FILE* fp, LOG_PARAMS) {

	asm_log_report();

	FILE_PTR_CHECK(fp);

	int fclose_return_value = fclose(fp);

	if (fclose_return_value == EOF)
		error_report(FCLOSE_ERR);

	return 0;
}

//===================================================================

int header_make_(struct Asmstruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	ASMSTRUCT_CHECK(asmstruct);

	asmstruct->header.signature = SIGN;
	asmstruct->header.version = VERSION;

	long file_size = asmstruct->ip - asmstruct->code;

	asmstruct->header.file_size = file_size;

	*(struct Header*)asmstruct->code = asmstruct->header;

	return 0;
}

//====================================================

int _write_code(struct Asmstruct* asmstruct, FILE* code_file, 
								                  LOG_PARAMS) {

	asm_log_report();

	FILE_PTR_CHECK(code_file);
	ASMSTRUCT_CHECK(asmstruct);

	int fwrite_ret = (int)fwrite(asmstruct->code, 
								sizeof(char), 
								(unsigned)asmstruct->header.file_size, 
								code_file);

	if (fwrite_ret != asmstruct->header.file_size) 
		error_report(FWRITE_ERR);

	return 0;
}

//====================================================

int open_listing_file_(const char* filename, LOG_PARAMS) {

	asm_log_report();

	extern FILE* Asm_listing_file;

	FILE* fp = fopen(filename, "wb");

	if (fp == NULL)
		error_report(FOPEN_ERROR);

	Asm_listing_file = fp;

	return 0;
}

//====================================================

int close_listing_file_(LOG_PARAMS) {

	asm_log_report();

	extern FILE* Asm_listing_file;

	int fclose_ret = fclose(Asm_listing_file);

	if (fclose_ret == EOF)
		error_report(FCLOSE_ERR);

	return 0;
}	

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions) \
	case CMD_##name: ;

void check_unique_(LOG_PARAMS) {

	asm_log_report();

	switch (0) {

		#include "../text_files/commands.txt"

		default:;
	}
}

#undef DEF_CMD_

//===================================================================

int write_listing(const char* string, unsigned int length, 
				  struct Asmstruct* asmstruct, int str_number) {

	extern FILE* Asm_listing_file;
	
	const unsigned char* printer = asmstruct->code + 
								   asmstruct->string_start_ips[str_number];

	long number = asmstruct->ip - printer;

	fprintf(Asm_listing_file, "Current position: %x. ", 
		   (unsigned)asmstruct->string_start_ips[str_number]);

	fprintf(Asm_listing_file,"Input string : %*s Written bytes: ", 
											 (int)length, string);

	for (long ct = 0; ct < number; ct++) {

		fprintf(Asm_listing_file, "%02x ", *(printer + ct));
	}

	fprintf(Asm_listing_file, "\n\n");

	return 0;
}

//=====================================================================

int fix_up_listing(int jump_pos, int jump_dest) {

	extern FILE* Asm_listing_file;

	fprintf(Asm_listing_file, "Filled jump destination. Ip: %x. "
							            "Destination Ip: %x \n\n",
											   (unsigned)jump_pos, 
											 (unsigned)jump_dest);
	return 0;
}

//=====================================================================

int _label_name_fix(char* name_buf, unsigned int* length,  
											LOG_PARAMS) {

	asm_log_report();

	name_buf[OPERATION_NAME_BUF_SIZE - 1] = '\0';

	if (name_buf[*length - 1] == ':') {

		name_buf[*length - 1] = '\0';
		*length -= 1;
	}

	return 0;
}

//=====================================================================

int _check_is_ram_using_command(struct Asm_string* asm_string, LOG_PARAMS) {

	asm_log_report();

	int byte_offset = 0;												
	char symbol = 0;	

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;											    
																	    	
	int scan_bracket = 													
		sscanf(string + byte_ct, " %c%n", &symbol, &byte_offset);  
																		
	if (scan_bracket == -1 || scan_bracket == 0) {						
																		
		print_incorrect_string(string, asm_string->str_number);	     				
		error_report(COMPILATION_ERR);							
	}																	
																		
	if (symbol == '[') {												
																		
		asm_string->oper_code = asm_string->oper_code | RAM_MASK;								
		asm_string->byte_ct += byte_offset;											
	}
	return 0;
}

//=====================================================================

int _check_second_square_bracket(struct Asm_string* asm_string, LOG_PARAMS) {

	asm_log_report();

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;

	if ((asm_string->oper_code & ~OPER_CODE_MASK) >= RAM_MASK) {                                                                                                                    
			
			if (*(string + byte_ct) != ']') {                             
																			
				print_incorrect_string(asm_string->string, 
									   asm_string->str_number);				                    	
				error_report(COMPILATION_ERR);						
			}	

			asm_string->byte_ct++;															
	}

	return 0;
}

//=====================================================================

int _read_argument(struct Asm_string* asm_string, struct Asmstruct* asmstruct,
																 LOG_PARAMS) {

	asm_log_report();
	ASMSTRUCT_PTR_CHECK(asmstruct);

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;
	unsigned char oper_code = asm_string->oper_code;

	if ((oper_code & ~OPER_CODE_MASK) == RAM_MASK) {                									    	
																	
		int is_var = read_var_arguments(asm_string, asmstruct);                      				    	
																	
		if (is_var == 1)								    
			return 0;
		if (is_var == -1)
			return -1;												    	
	}                                                               	
																	
	elem_t elem_t_value = 0;                                            
																	
	int sscanf_ret = sscanf(string + byte_ct, ELEM_SPEC, 				
											&elem_t_value);			
	
	if (sscanf_ret == 1) {                                          	
																	
		int read_elem_t = read_elem_t_argument(asm_string, 
											   asmstruct);								    
		
		if (read_elem_t == -1)
			return -1; 

		return 0;                                                  	
	}                                                               	
																	
	char register_number = 0;                                       	
	sscanf_ret = sscanf(string + byte_ct, "%c", &register_number);  	
																	
	if (sscanf_ret == 1) {                                          	
																	
		int read_register = read_register_number(asm_string, 
												 asmstruct);									    	
		if (read_register == -1)
			return -1; 

		return 0;                                                    	
	}                                                               	
																	
	if (sscanf_ret == -1 || sscanf_ret == 0) {                       	
																	
		print_incorrect_string(asm_string->string, 
						   asm_string->str_number);						                   	
		
		error_report(COMPILATION_ERR);						
	}

	return 0;                                                                	
}

//=====================================================================

int _read_var_arguments(struct Asm_string* asm_string, struct Asmstruct* asmstruct, 
																		LOG_PARAMS) {
	asm_log_report();

	char register_number = 0;											
	char sign = 0;														
	elem_t elem_t_value = 0;											
	int byte_offset = 0;

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;												
																			
	int sscanf_var = sscanf(string + byte_ct, " %cx %c " ELEM_SPEC "%n",
											    &register_number, &sign,  
											&elem_t_value, &byte_offset);  
																			
	if (sscanf_var != 3 || (sign != '-' && sign != '+'))			
		return 0;															
																			
	asm_string->oper_code = asm_string->oper_code | IMM_MASK;									
	asm_string->oper_code = asm_string->oper_code | REGISTER_MASK;								
																			
	register_number = (char)(register_number -  Ascii_offset);			
																			
	if (register_number < 1 || register_number > REGISTER_SIZE) {		
																			
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);											
		error_report(INV_REGISTER_NUMBER);						
	}																	
																			
	*asmstruct->ip = (unsigned char)register_number;					
	asmstruct->ip++;													
																		
	ASMSTRUCT_IP_CHECK(asmstruct);										
																			
	if (sign == '-')													
		elem_t_value *= -1;												
																			
	*(elem_t*)asmstruct->ip = elem_t_value;							
	asmstruct->ip += sizeof(elem_t);									
																			
	ASMSTRUCT_IP_CHECK(asmstruct);									
																			
	asm_string->byte_ct += byte_offset;										
	return 1;												
}

//=====================================================================

int _read_elem_t_argument(struct Asm_string* asm_string, struct Asmstruct* asmstruct, 
																		LOG_PARAMS) {
	asm_log_report();
	ASMSTRUCT_PTR_CHECK(asmstruct);

	asm_string->oper_code = asm_string->oper_code | IMM_MASK;                          			
																		
	int byte_offset = 0;
	elem_t elem_t_value = 0;    

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;                                    		
																		
	int sscanf_ret = sscanf(string + byte_ct, " " ELEM_SPEC " %n",  		
								 	&elem_t_value, &byte_offset);         
																		
	if (sscanf_ret != 1) {                                        		
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);						               		
		error_report(COMPILATION_ERR);							
	}                                                            		
																		
	asm_string->byte_ct += byte_offset;                                     		
																		
	*(elem_t *)asmstruct->ip = elem_t_value;                      		
	asmstruct->ip += sizeof(elem_t);                            		
																		
	ASMSTRUCT_CHECK(asmstruct);  

	return 0;                               		
}

//=====================================================================

int _read_register_number(struct Asm_string* asm_string, struct Asmstruct* asmstruct,
																		 LOG_PARAMS) {

    asm_log_report();
	ASMSTRUCT_PTR_CHECK(asmstruct);

	asm_string->oper_code = asm_string->oper_code | REGISTER_MASK;                      		

	char register_number = 0;
	int byte_offset = 0; 

	const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;                                       		
																		
	int sscanf_ret = sscanf(string + byte_ct, " %cx %n",            		
						&register_number, &byte_offset);        		
																		
	if (sscanf_ret != 1) {                                        		
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);					               			
		error_report(COMPILATION_ERR);							
	}	 

	asm_string->byte_ct += byte_offset;                                     	
	register_number = (char)(register_number -  Ascii_offset);          
																		
	if (register_number < 1 || register_number > REGISTER_SIZE) {		
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);							               		
		error_report(INV_REGISTER_NUMBER);					
	}	                                                        		
																		
	*asmstruct->ip = (unsigned char)register_number;                    
	asmstruct->ip++;                                            		
																		
	ASMSTRUCT_CHECK(asmstruct);  

	return 0;                               		
}

//=====================================================================

int _read_label_name(struct Asmstruct* asmstruct, struct Asm_string* asm_string, 
								  				   const char* ptr, LOG_PARAMS) {
    asm_log_report();

	ASMSTRUCT_PTR_CHECK(asmstruct);

	char label_name_buf[LABEL_NAME_SIZE] = { 0 };                       
	int byte_start = 0;                                                 	
	int byte_end = 0;                                                   	
																		
	int sscan_label  = sscanf(ptr + 1, " %n%s%n", &byte_start,          
									label_name_buf, &byte_end);            
																		
	if (sscan_label == 0) {												
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);											
		error_report(COMPILATION_ERR);							
	}																	
																		
	if (byte_end - byte_start > LABEL_NAME_SIZE) {                      
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);											
		error_report(LABEL_NAME_TOO_THICK);                      
	}																	
																		
	int64_t label_hash =                                                
		get_hash(label_name_buf, (unsigned)(byte_end - byte_start));    
																		
	int code_pos = search_label_name(label_hash, asmstruct);            
																		
	if (code_pos == -1) {												
																		
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);											
		error_report(SEARCH_LABEL_NAME_ERR);						
	}																	
																		
	if (code_pos == -2) {                  	                            
																		
		int ret = add_ud_label(label_name_buf, (int)(asmstruct->ip 	    
													- asmstruct->code), 
														asmstruct); 		
		if (ret == -1) {												
																		
			print_incorrect_string(asm_string->string, 
								   asm_string->str_number);									
			error_report(ADD_UNKNOWN_DEST_JUMP_ERR);				
		}																
	}                                                                   
																		
	else                                                                
		*(int *)asmstruct->ip = code_pos;                               
																		
	asmstruct->ip += sizeof(int);                                       
	ASMSTRUCT_CHECK(asmstruct);    

    const char* string = asm_string->string;
	int byte_ct = asm_string->byte_ct;

	asm_string->byte_ct += (int)(ptr + 1 - (string + byte_ct - 1) + byte_end);  

	return 0;    
}

//=====================================================================

int _read_imm_label(struct Asmstruct* asmstruct, struct Asm_string* asm_string, 
																	LOG_PARAMS) {
	asm_log_report();
	ASMSTRUCT_PTR_CHECK(asmstruct);

	int byte_offset = 0;                                           			
	int dest_str = 0;      

	const char* string = asm_string->string;	
	int byte_ct = asm_string->byte_ct;                                        		
                                                                       		
	int scan_imm = sscanf(string + byte_ct,                          	
							" %d%n", &dest_str, &byte_offset);         		
                                                                       		
	if (scan_imm == -1) {                                        		
																			
		print_incorrect_string(asm_string->string, 
							   asm_string->str_number);						               		
		error_report(COMPILATION_ERR);							
	}																	
                                                                       		
	if (dest_str < 1 || dest_str > asmstruct->strings_number)      		
		error_report(INV_DESTINATION_OF_JMP);               		
                                                                       		
	if (asm_string->str_number >= dest_str) {                                                              		
			
		int dest_ip = get_string_start_ip(dest_str, asmstruct);    		
                                                                       		
		if (dest_ip == -1) {											
																			
			print_incorrect_string(asm_string->string, 
						   		   asm_string->str_number);									
			error_report(COMPILATION_ERR);						
		}																
                                                                       		
		*(int *)asmstruct->ip = dest_ip;                           	
	}                                                              		
                                                                       		
	else {                                                              		
			
		int ret = add_string_jump((int)(asmstruct->ip 					
							        - asmstruct->code), 				    
 							  	  dest_str, asmstruct);            		
																			
		if (ret == -1) {											
																			
			print_incorrect_string(asm_string->string, 
						   	       asm_string->str_number);											
			error_report(ADD_STRING_JUMP_ERR);					
		}																
	}
	                                                              	
    asmstruct->ip += sizeof(int);                                  		
	ASMSTRUCT_CHECK(asmstruct);                                    		
                                                                       		
	asm_string->byte_ct += byte_offset; 

	return 0;                                       		
}