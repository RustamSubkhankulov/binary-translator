#pragma once

#include "processor_general.h"
#include "../../text_processing/text_processing.h"
#include "../../logs/errors_and_logs.h"

//===================================================================

#ifdef ASM_LOGS

	#define asm_log_report() \
		    log_report()

#else

	#define asm_log_report() \
			dummy_func()

#endif

//===================================================================

#define STRING_IS_NOT_LABEL -2

//===================================================================

struct Asm_string 
{
	const char* string;

	int byte_ct;
	int str_number;

	unsigned char oper_code;
};

//===================================================================

struct Str_jump
{

	int string_number;
	int code_ip;
};

//===================================================================

struct Label
{

	int64_t label_hash;
	int code_pos;
};

//===================================================================

struct Label_temp
{

	char* label_name;
	int label_name_length;

	int str_number;
};

//===================================================================

struct Asmstruct
{

	unsigned char *code;
	unsigned char *ip;

	struct Header header;

	struct Label *labels;
	int labels_number;
	int labels_capacity;

	//Unknown destination jumps
	struct Label *ud_jumps;
	int ud_jumps_number;
	int ud_jumps_capacity;

	//Jumps by the string number
	struct Str_jump *str_jumps;
	int str_jumps_number;
	int str_jumps_capacity;

	//Instruction pointer values for the start of each string
	int *string_start_ips;
	int strings_number;
};

//===================================================================

#define ASM_CHECK_COMMAND_LINE_ARGS(argc) {									\
																			\
		if (argc != ASM_NUM_OF_CMND_LINE_ARGS){								\
																			\
			fprintf(stderr, "Invalid number of command line arguments\n");	\
																			\
			fprintf(stderr, "Number of arguments expecter: %d.", 			\
									  ASM_NUM_OF_CMND_LINE_ARGS);			\
			fprintf(stderr, "Number of arguments entered: %d", argc);		\
																			\
			return -1;														\
		}																	\
	}

//===================================================================

#define ASMSTRUCT_CHECK(asmstruct)                               			\
	{                                                            			\
                                                                 			\
		if (asmstruct == NULL)                                   			\
			error_report(ASMSTRUCT_INV_PTR);              			\
                                                                 			\
		if (asmstruct->code == NULL)                             			\
			error_report(ASMSTRUCT_EMPTY_CODE_ARR);       			\
                                                                 			\
		if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE) 			\
			error_report(ASMSTRUCT_INV_IP);               			\
                                                                 			\
		HEADER_PTR_CHECK(&asmstruct->header);                    			\
	}

//===================================================================

#define ASMSTRUCT_IP_CHECK(asmstruct)                            			\
	{                                                            			\
                                                                 			\
		if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE) 			\
			error_report(ASMSTRUCT_INV_IP)                			\
	}

//===================================================================

#define ASMSTRUCT_PTR_CHECK(asmstruct)              						\
	{                                               						\
                                                    						\
		if (asmstruct == NULL)                      						\
			error_report(INV_ASMSTRUCT_PTR); 						\
	}

//===================================================================

#define HEADER_PTR_CHECK(header)              								\
	{                                         								\
                                              								\
		if (header == NULL)                   								\
			error_report(INV_HDR_PTR); 								\
	}

//===================================================================

#define TEXT_PTR_CHECK(text)                   								\
	{                                          								\
                                               								\
		if (text == NULL)                      								\
			error_report(INV_TEXT_PTR); 								\
	}

//===================================================================

#define CHAR_PTR_CHECK(string)                								\
	{                                         								\
                                              								\
		if (string == NULL)                   								\
			error_report(NULL_STRING); 								\
	}

//===================================================================

#define CHECK_IF_STRING_IS_EMPTY(string)              						\
	{                                                 						\
                                                      						\
		char ch = 0;                                  						\
		int ret = sscanf((string), " %c", &ch);      						\
                                                      						\
		if (ret > 0)                                  						\
			error_report(COMPILATION_ERR); 							\
	}

//===================================================================

#define scan_label(string, label_temp) \
       _scan_label(string, label_temp, LOG_ARGS)

#define read_argument(asm_string, asmstruct) \
	   _read_argument(asm_string, asmstruct, LOG_ARGS)

#define read_var_arguments(asm_string, asmstruct) \
	   _read_var_arguments(asm_string, asmstruct, LOG_ARGS)

#define label_name_fix(operation_name_buf, length) \
       _label_name_fix(operation_name_buf, length, LOG_ARGS)

#define read_elem_t_argument(asm_string, asmstruct) \
	   _read_elem_t_argument(asm_string, asmstruct, LOG_ARGS)

#define read_register_number(asm_string, asmstruct) \
	   _read_register_number(asm_string, asmstruct, LOG_ARGS)

#define read_int_argument(asm_string, asmstruct) \
	   _read_int_argument(asm_string, asmstruct, LOG_ARGS)

#define read_label_name(asmstruct, asm_string, ptr) \
	   _read_label_name(asmstruct, asm_string, ptr, LOG_ARGS)

#define read_imm_label(asmstruct, asm_string) \
	   _read_imm_label(asmstruct, asm_string, LOG_ARGS)

//===================================================================

#define check_unigue() \
		check_unique_(LOG_ARGS)

//===================================================================

#define open_listing_file(filename) \
		open_listing_file_(filename, LOG_ARGS)

#define close_listing_file() \
		close_listing_file_(LOG_ARGS)

#define clear_opernamebuf() \
		clear_opernamebuf_(LOG_ARGS)

#define text_remove_comments(text) \
		text_remove_comments_(text, LOG_ARGS)

//===================================================================

#define asmstruct_allocate_memory(number, asmstruct) \
	   _asmstruct_allocate_memory(number, asmstruct, \
							   			   LOG_ARGS)

#define convert_operations_to_binary(text, asmstruct)  \
		convert_operations_to_binary_(text, asmstruct, \
								  		     LOG_ARGS)

#define read_label(string, asmstruct, str_number) \
	    read_label_(string, asmstruct, str_number, LOG_ARGS)

#define add_label(asmstruct, label_hash) \
	   _add_label(asmstruct, label_hash, \
			                   LOG_ARGS)

#define search_label_name(hash, asmstruct) \
	   _search_label_name(hash, asmstruct, \
					             LOG_ARGS)

#define add_ud_label(string, ip, asmstruct) \
	   _add_ud_label(string, ip, asmstruct, \
				                  LOG_ARGS)

#define add_string_jump(code_ip, dest_string, asmstruct) \
	   _add_string_jump(code_ip, dest_string, asmstruct, \
					                           LOG_ARGS)

#define process_string(string, asmstruct, str_number)  \
	    process_string_(string, asmstruct, str_number, \
											LOG_ARGS)

#define save_string_start_ip(asmstruct, number) \
	   _save_string_start_ip(asmstruct, number, \
						              LOG_ARGS)

#define jumps_fill_gaps(asmstruct) \
	   _jumps_fill_gaps(asmstruct, LOG_ARGS)

#define get_string_start_ip(str_number, asmstruct) \
	   _get_string_start_ip(str_number, asmstruct, \
						                 LOG_ARGS)

#define asmstruct_free_memory(asmstruct) \
	   _asmstruct_free_memory(asmstruct, LOG_ARGS)

#define print_incorrect_string(string, str_number) \
	   _print_incorrect_string(string, str_number, LOG_ARGS)

//===================================================================

#define open_code_file(filename, mode) \
	    open_code_file_(filename, mode, LOG_ARGS)

#define close_code_file(fp) \
	    close_code_file_(fp, LOG_ARGS)

#define write_code(asmstruct, code_file) \
	   _write_code(asmstruct, code_file, LOG_ARGS)

#define header_make(asmstruct) \
	    header_make_(asmstruct, LOG_ARGS)

#define label_name_fix(operation_name_buf, length) \
	   _label_name_fix(operation_name_buf, length, LOG_ARGS)

#define check_is_ram_using_command(asm_string) \
	   _check_is_ram_using_command(asm_string, LOG_ARGS);

#define check_second_square_bracket(asm_string) \
	   _check_second_square_bracket(asm_string, LOG_ARGS);

//========================================================================

int clear_opernamebuf_(LOG_PARAMS);

//========================================================================

int text_remove_comments_(Text *text, LOG_PARAMS);

int convert_operations_to_binary_(Text *text, struct Asmstruct *asmstruct,
								                              LOG_PARAMS);

int _save_string_start_ip(struct Asmstruct *asmstruct, int number,
						                              LOG_PARAMS);

int process_string_(const char *string, struct Asmstruct *asmstruct,
					                    int str_number, LOG_PARAMS);

int read_label_(const char *string, struct Asmstruct *asmstruct, 
									 int str_number,LOG_PARAMS);

int _asmstruct_allocate_memory(long number, struct Asmstruct *asmstruct,
							                                LOG_PARAMS);

int _asmstruct_free_memory(struct Asmstruct *asmstruct, LOG_PARAMS);

//========================================================================

FILE *open_code_file_(const char *filename, const char *mode,
					                             LOG_PARAMS);

int close_code_file_(FILE *fp, LOG_PARAMS);

int _write_code(struct Asmstruct *asmstruct, FILE *code_file, LOG_PARAMS);
 
//========================================================================

int header_make_(struct Asmstruct *asmstruct, LOG_PARAMS);

//========================================================================

int _search_label_name(int64_t label_hash, struct Asmstruct *asmstruct,
					                                	   LOG_PARAMS);

int _add_ud_label(const char *string, int ip, struct Asmstruct *asmstruct,
				                                              LOG_PARAMS);

int _add_label(struct Asmstruct *asmstruct, int64_t label_hash,
			                                       LOG_PARAMS);

int _add_string_jump(int code_ip, int dest_string, struct Asmstruct *asmstruct,
					                                               LOG_PARAMS);

//========================================================================

int _jumps_fill_gaps(struct Asmstruct *asmstruct, LOG_PARAMS);

int _get_string_start_ip(int str_number, struct Asmstruct *asmstruct,
						                                 LOG_PARAMS);

//=======================================================================

void print_labels(struct Asmstruct *asmstruct);

int open_listing_file_(const char* filename, LOG_PARAMS);

int close_listing_file_(LOG_PARAMS);

int write_listing(const char* string, unsigned int length, 
				  struct Asmstruct* asmstruct, int str_number);

int fix_up_listing(int jump_pos, int jump_dest);

void check_unique_(LOG_PARAMS);

int _print_incorrect_string(const char* string, int str_number, 
												   LOG_PARAMS);

//=======================================================================

int _scan_label(const char* string, struct Label_temp* label_temp, 
													   LOG_PARAMS);

int _label_name_fix(char* name_buf, unsigned int* length,
											 LOG_PARAMS);

int _check_is_ram_using_command(struct Asm_string* asm_string, LOG_PARAMS);

int _check_second_square_bracket(struct Asm_string* asm_string, LOG_PARAMS);

int _read_argument(struct Asm_string* asm_string, struct Asmstruct* asmstruct,
																  LOG_PARAMS);

int _read_var_arguments(struct Asm_string* asm_string, struct Asmstruct* asmstruct, 
																		LOG_PARAMS);

int _read_elem_t_argument(struct Asm_string* asm_string, struct Asmstruct* asmstruct, 
																		LOG_PARAMS);

int _read_int_argument(struct Asm_string* asm_string, struct Asmstruct* asmstruct, 
																	  LOG_PARAMS);

int _read_register_number(struct Asm_string* asm_string, struct Asmstruct* asmstruct,
																		 LOG_PARAMS);

int _read_label_name(struct Asmstruct* asmstruct, struct Asm_string* asm_string, 
								  				   const char* ptr, LOG_PARAMS);

int _read_imm_label(struct Asmstruct* asmstruct, struct Asm_string* asm_string, 
																	LOG_PARAMS);