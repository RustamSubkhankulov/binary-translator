#include <stdlib.h>

#include "global_conf.h"
#include "hash/hash.h"
#include "list/list.h"

//===============================================

int main(int argc, char* argv[])
{
    #ifdef LOGS

        FILE* logs_file = open_log_file(Logfile_name);
        if (!logs_file)
            return -1;

    #endif 

    int ret_val = 0;

    // ret_val = hash_table_compare_hash_func("text_files/res.txt", 
    //                                        "text_files/oxford.txt");
    // if (ret_val == -1) return -1;

    ret_val = hash_table_stress_test("text_files/oxford.txt", my_hash);
    if (ret_val == -1) return -1;

    #ifdef LOGS
        close_log_file();
    #endif 

    return 0;
}