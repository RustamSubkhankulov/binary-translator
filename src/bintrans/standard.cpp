#include <math.h>

#include "../general/general.h"
#include "standard.h"

//===============================================

#ifdef FILE_IO

    static FILE* Input = NULL;

    static FILE* Output = NULL;

#endif 

//===============================================

float standard_sin (float arg)
{
    return sinf(arg);
}

float standard_cos (float arg)
{
    return cosf(arg);
}

float standard_tg  (float arg)
{
    return tanf(arg);
}

float standard_ln  (float arg)
{
    return logf(arg);
}

float standard_asin(float arg)
{
    return asinf(arg);
}

float standard_atg (float arg)
{
    return atanf(arg);
}

float standard_pow (float base, float exp)
{
    return pow(base, exp);
}

float standard_fabs(float arg)
{
    return fabs(arg);
}

//-----------------------------------------------

float standard_in(void)
{
    float read = 0;

    #ifdef FILE_IO

        int fscanf_return = fscanf(Output, "%f\n", &read);
        if (fscanf_return != 1)
        {
            fprintf(stderr, "Error occured during input from file");
        }

    #else

        int scanf_return = scanf(" %f", &read); 
        while (getchar() != '\n') {;}
        if (scanf_return != 1)
        {
            fprintf(stderr, "Error occured during input from terminal");
        }

    #endif

    return read;
}

//===============================================

int standard_out(float argument)
{
    #ifdef FILE_IO

        fprintf(Output, "%f\n", argument);

    #else

        printf("%f", argument);

        //printf("%f\n", argument);
        
    #endif

    return 0; 
}

//===============================================

#ifdef FILE_IO

    int _init_file_io(const char* input_name, 
                      const char* output_name
                      FOR_LOGS(, LOG_PARAMS))
    {
        standard_log_report();

        assert(input_name);
        assert(output_name);

        Input = open_file(input_name, "rb");
        if (!Input) return -1;

        Output = open_file(output_name, "wb");
        if (!Output) return -1;

        return 0;
    }

//===============================================

    int _close_file_io(FOR_LOGS(LOG_PARAMS))
    {
        standard_log_report();

        int ret_val = 0;

        ret_val = close_file(Input);
        if (ret_val == -1) return -1;

        ret_val = close_file(Output);
        if (ret_val == -1) return -1;

        Input  = NULL;
        Output = NULL;
    
        return 0;
    }

#endif 