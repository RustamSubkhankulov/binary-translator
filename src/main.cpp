#include <stdlib.h>

#include "global_conf.h"
#include "bintrans/bintrans.h"

//===============================================

int main(int argc, char* argv[])
{
    #ifdef LOGS

        FILE* logs_file = open_log_file(Logfile_name);
        if (!logs_file)
            return -1;

    #endif 

    int ret_val = 0;

    ret_val = binary_translate();
    if (ret_val == -1)  return -1;

    #ifdef LOGS
        close_log_file();
    #endif 

    return 0;
}