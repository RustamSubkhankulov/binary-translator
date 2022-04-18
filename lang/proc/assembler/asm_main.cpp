#include "assembler.h"
#include "../../general/general.h"

int main(int argc, char* argv[]) {

	ASM_CHECK_COMMAND_LINE_ARGS(argc);
	
	FILE* logs_file = open_log_file(argv[3]);
	if (logs_file == NULL)
		return -1;

	int err_val = 0;

	err_val = open_listing_file(argv[4]); $

	struct Text text = { 0 };

	char* text_buf = text_init_(argv[1], &text);
	text_remove_comments(&text); $
	
	struct Asmstruct asmstruct = { 0 };

	err_val = convert_operations_to_binary(&text, &asmstruct); $
	err_val = header_make(&asmstruct); $                               

	FILE* code_file = open_code_file(argv[2], "wb"); 
	FILE_PTR_CHECK(code_file);

	err_val = write_code(&asmstruct, code_file); $
	err_val = asmstruct_free_memory(&asmstruct); $
	err_val = close_code_file(code_file); $

	err_val = text_clear_mem(&text, text_buf); $

	err_val = close_listing_file();
	err_val = close_log_file(); $

	return 0;
}