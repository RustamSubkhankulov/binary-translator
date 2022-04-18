#pragma once

#define LOG_PARAMS \
        const char* ___func_name, const char* ___file_name, int ___line, FILE* logs_file

#define LOG_ARGS \
        __FUNCTION__, __FILE__, __LINE__, logs_file

#define LOGS_ARGS_USE \
        ___func_name, ___file_name, ___line, logs_file
;