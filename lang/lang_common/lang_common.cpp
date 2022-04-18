#include <stdlib.h>
#include <string.h>

#include "lang_common.h"
#include "../general/general.h"

//=========================================================

int _code_read_console(Buffer_struct* buffer, FOR_LOGS( LOG_PARAMS)) {

    lang_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer);

    char* buf = (char*)calloc(Console_input_buf_size, sizeof(char));
    if (!buf)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    char* scanned = fgets(buf, Console_input_buf_size, stdin);
    if  (!scanned)
    {
        error_report(CONSOLE_READ_ERROR);
        return -1;
    }

    buf[strlen(buf) - 1] = '\0';

    while (strlen(buf) == 0) 
    {
        clean_buffer(buf, Console_input_buf_size);

        fgets(buf, Console_input_buf_size, stdin);
        buf[strlen(buf) - 1] = '\0';
    }

    buffer_struct_init(buffer, buf, (int)strlen(buf), 0);

    return 0;
}

//===================================================================

int _code_read_file(Buffer_struct* buffer, const char* filename FOR_LOGS(, LOG_PARAMS)) {

    lang_log_report();
    FILENAME_CHECK(filename);
    BUFFER_STRUCT_PTR_CHECK(buffer);

    int size = 0;

    char* buf = copy_from_file_to_buffer(filename, &size);
    if (!buf)
        return -1;
    string_prepare(buf, size);

    buffer_struct_init(buffer, buf, size, 0);

    return 0;
}

//===================================================================

int _assemble_and_execute(const char* asm_code FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    FILENAME_CHECK(asm_code);

    char cmnd_buffer[System_cmnd_buffer_size] = { 0 };

    sprintf(cmnd_buffer, "./proc/asm.exe %s txt_files/trans.code asm_logs.html txt_files/trans_listing.txt ", asm_code);

    int ret = system(cmnd_buffer);
    if (ret != 0)
        return -1;

    ret = system("./proc/cpu.exe txt_files/trans.code txt_files/in.txt txt_files/out.txt proc_logs.html");
    if (ret != 0)
        return -1;

    return 0;
}