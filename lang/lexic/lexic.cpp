#include <string.h>
#include <math.h>
#include <ctype.h>

#include "lexic.h"
#include "../general/general.h"
//#include "../syntax/syntax_id.h"
//#include "../syntax/func_definitions.h"

//===================================================================

struct Tokens* _lexic_analysis(Buffer_struct* buffer FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    if (!buffer)
    {
        error_report(BUFFER_STRUCT_INV_PTR);
        return NULL;
    }

    struct Tokens* tokens = tokens_struct_allocate();
    if (!tokens)
        return NULL;

    tokens->capacity = Token_start_number;

    while(POS < buffer->size)
    {
        if (tokens->amount == tokens->capacity)
        {
            int ret = token_array_resize(tokens);
            if (ret == -1)
                return NULL;
        }

        int is_ok = read_token(buffer, tokens);
        if (is_ok == -1)
            return NULL;
    }

    int ret = delete_comments(tokens);
    if (ret == -1)
        return NULL;

    #ifdef TOKENS_DUMP 

        ret = tokens_dump(tokens, logs_file);
        RETURN_CHECK(ret);

    #endif

    return tokens;
}

//===================================================================

int _token_array_resize(struct Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    size_t prev_size = (size_t) tokens->capacity;
    size_t new_size  = (size_t)(tokens->capacity * 2);

    tokens->capacity *= 2;

    tokens->token_array = (Token**)my_recalloc((void*)tokens->token_array, new_size, prev_size, 
                                                                       sizeof(struct Token*));
    if (!tokens->token_array)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    return 0;
}

//===================================================================

int _read_token(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer);
    TOKENS_STRUCT_PTR_CHECK(tokens);

    skip_blank_symb(buffer);

    if (IS_OPERAND(*BUF))
        return read_operand(buffer, tokens);

    else if (IS_NUMBER(*BUF))
        return read_number(buffer, tokens);

    else if (IS_COMPARISON_SYMB(*BUF))
        return read_comparison_sign(buffer, tokens);

    else
    {
        int ret = read_id(buffer, tokens);
        RETURN_CHECK(ret);

        if (!ret)
        {
            error_report(UNKNOWN_TOKEN);
            buffer_dump(buffer);
            return -1;
        }
    }

    return 0;
}

//===================================================================

int _read_comparison_sign(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    char cmp_sign_buf[2] = { 0 };

    int counter = 0;

    while(IS_COMPARISON_SYMB(*BUF) && counter < 2)
    {
        cmp_sign_buf[counter++] = *BUF;
        POS++;
    }

    int code = cmp_sign_code(cmp_sign_buf);
    if (!code)
    {
        SYNTAX_ERROR(buffer);
        return -1;
    }

    ADD_CMP_SIGN_TOKEN(tokens, code);

    return 0;
}

//===================================================================

int _read_number(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    double val = 0;
    int counter = 0;

    while(IS_NUMBER(*BUF))
    {
        val = val * 10 + (*BUF - '0');
        counter++;
        POS++;
    }

    if (*BUF == '.')
    {
        POS++;

        while(IS_NUMBER(*BUF) && counter < 6)
        {
            val += ((double)(*BUF - '0'))/pow(10, counter);
            POS++;
            counter++;
        }

        if (!counter)
        {
            SYNTAX_ERROR(buffer);
            return -1;
        }
    }

    ADD_CONSTANT_TOKEN(tokens, val);
    return 0;
}

//===================================================================

int _read_id(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    char id_buf[Id_lenght] = { 0 };

    int is_scanned = scan_id(buffer, id_buf);
    RETURN_CHECK(is_scanned);

    if (!is_scanned)
        return 0;

    int code = 0;

    code = calc_func_code(id_buf);
    if (code)
    {
        ADD_CALC_FUNC_TOKEN(tokens, code);
        return 1;
    }

    code = std_func_code(id_buf);
    if (code)
    {
        ADD_STD_FUNC_TOKEN(tokens, code);
        return 1;
    }

    code = key_word_code(id_buf);
    if (code)
    {
        ADD_KEY_WORD_TOKEN(tokens, code);
        return 1;
    }

    double val = constant_value(id_buf);
    if (!double_is_equal(val, 0))
    {
        ADD_CONSTANT_TOKEN(tokens, val);
        return 1;
    }

    ADD_ID_TOKEN(tokens, get_hash(id_buf, strlen(id_buf)));
    return 1;
}

//===================================================================

int _scan_id(Buffer_struct* buffer, char* id_buf FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    int counter = 0;

    if (IS_ALPHA(*BUF))
    {
        id_buf[counter++] = *BUF;
        POS++;
    }

    while(IS_ALPHA(*BUF) || IS_NUMBER(*BUF))
    {
        id_buf[counter++] = *BUF;
        POS++;
    }

    return (counter)? 1: 0;
}

//===================================================================

int _read_operand(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    char oper = *BUF;
    POS++;

    ADD_OPERAND_TOKEN(tokens, oper);
    
    return 0;
}

//===================================================================

int _add_operand_token(Token* token, char operand FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = OPER;
    token->data.operand = operand;

    return 0;
}

//-------------------------------------------------------------------

int _add_constant_token(Token* token, double constant FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = CONST;
    token->data.constant = constant;

    return 0;
}

//-------------------------------------------------------------------

int _add_key_word_token(Token* token, int code  FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = KEY_WORD;
    token->data.key_word_code = code;

    return 0;
}

//-------------------------------------------------------------------

int _add_cmp_sign_token(Token* token, int code FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = CMP_SIGN;
    token->data.cmp_sign_code = code;

    return 0;
}

//-------------------------------------------------------------------

int _add_calc_func_token(Token* token, int code FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = CALC_FUNC;
    token->data.calc_func_code = code;

    return 0;
}

//-------------------------------------------------------------------

int _add_id_token(Token* token, int64_t hash FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = ID;
    token->data.id_hash = hash;

    return 0;
}

//-------------------------------------------------------------------

int _add_std_func_token(Token* token, int code FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    TOKEN_PTR_CHECK(token);

    token->type = STD_FUNC;
    token->data.std_func_code = code;

    return 0;
}

//===================================================================

struct Tokens* _tokens_struct_allocate(FOR_LOGS(LOG_PARAMS))
{
    lang_log_report();

    struct Tokens* tokens_struct_ptr = (struct Tokens*)calloc(sizeof(struct Tokens), 
                                                              sizeof(char));

    if (!tokens_struct_ptr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    tokens_struct_ptr->token_array = (struct Token**)calloc(Token_start_number, 
                                                            sizeof(struct Token*));

    if (!tokens_struct_ptr->token_array)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    return tokens_struct_ptr;
}

//===================================================================

struct Token* _token_allocate( FOR_LOGS( LOG_PARAMS)) 
{
    lang_log_report();

    struct Token* token_ptr = (struct Token*)calloc(sizeof(struct Token), sizeof(char));

    if (!token_ptr)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    return token_ptr;
}

//===================================================================

int _std_func_code(const char* id FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();

    if (!id)
    {
        error_report(IS_FUNC_INV_ARG);
        return -1;
    }

    for (int counter = 0; counter < Std_functions_number; counter++)
    {
        if (!strcmp(id, Std_functions[counter].name))
            return Std_functions[counter].code;
    }

    return 0;
}

//===================================================================

int _calc_func_code(const char* id FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    if (!id) 
    {
        error_report(IS_FUNC_INV_ARG);
        return -1;
    }

    for (int counter = 0; counter < Calc_functions_number; counter++)
    {
        if (!strcmp(id, Calc_functions[counter].name))
            return Calc_functions[counter].code;
    }

    return 0;
}

//===================================================================

int _cmp_sign_code(const char* id FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    if (!id)
    {
        error_report(INV_STRING_ARG);
        return -1;
    }

    for (int counter = 0; counter < Std_cmp_signs_number; counter++)
    {
        if (!strncmp(id, Std_cmp_signs[counter].name , 2))
            return Std_cmp_signs[counter].code;
    }

    return 0;
}

//====================================================================

double _constant_value(const char* name FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    if (!name) {

        error_report(IS_CONST_INV_ARG);
        return -1;
    }

    for (int counter = 0; counter < Std_constants_number; counter++)
    {
        if (!strcmp(name, Std_constants[counter].name))
            return Std_constants[counter].value;
    }

    return 0;
}

//=====================================================================

int _key_word_code(const char* id FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    if (!id)
    {
        error_report(INV_STRING_ARG);
        return -1;
    }

    for (int counter = 0; counter < Key_words_number; counter++) 
    {
        if (!strcmp(id, Key_words[counter].name))
            return Key_words[counter].key_word_code;
    }

    return 0;
}

//===================================================================

int _skip_blank_symb(Buffer_struct* buffer FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer);

    while(isblank(*BUF)) POS++;
    
    return 0;
}

//==================================================================

int _free_tokens(Tokens* tokens FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();

    for (int counter = 0; counter < tokens->amount; counter++)
    {
        free(tokens->token_array[counter]);
    }

    free(tokens->token_array);
    free(tokens);

    return 0;
}

//==================================================================

int _delete_comments(Tokens* tokens FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    TOKENS_STRUCT_PTR_CHECK(tokens);

    // tokens_dump(tokens, logs_file);
    // fflush(logs_file);

    int open  = 0;
    int close = 0;

    while ((open = find_cmnt_open_token(tokens)) != NO_TOKEN)
    {
        close = find_cmnt_close_token(tokens);

        if (close < open)
        {
            error_report(CMNT_CLOSE_BEFORE);
            return -1;
        }
        
        if (close == NO_TOKEN)
        {
            error_report(CMNT_UNCLOSED);
            return -1;
        }

        int comment_len = close - open + 1;

        for (int ct = 0; ct < comment_len; ct++)
            free(tokens->token_array[open + ct]);

        memmove((tokens->token_array) + open, (tokens->token_array) +  close + 1,
                              sizeof(Token*) * (size_t)(tokens->amount - close));

        tokens->amount -= comment_len;

        tokens->token_array = (Token**)realloc(tokens->token_array, (size_t)(tokens->amount) * sizeof(Token*));
    }

    if (find_cmnt_close_token(tokens) != NO_TOKEN)
    {
        error_report(CMNT_CLOSE_BEFORE);
        return -1;
    }

    return 0;
}

//==================================================================

int _find_cmnt_open_token(Tokens* tokens FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    TOKENS_STRUCT_PTR_CHECK(tokens);

    for (int counter = 0; counter < tokens->amount; counter++)
    {
        if (tokens->token_array[counter]->type == KEY_WORD 
         && tokens->token_array[counter]->data.key_word_code == CMNT_OPEN)
        {
            return counter;
        }
    }

    return NO_TOKEN;
}

//==================================================================

int _find_cmnt_close_token(Tokens* tokens FOR_LOGS(, LOG_PARAMS))
{
    lang_log_report();
    TOKENS_STRUCT_PTR_CHECK(tokens);

    for (int counter = 0; counter < tokens->amount; counter++)
    {
        if (tokens->token_array[counter]->type == KEY_WORD 
         && tokens->token_array[counter]->data.key_word_code == CMNT_CLOSE)
        {
            return counter;
        }
    }

    return NO_TOKEN;
}

//==================================================================

int _tokens_dump(Tokens* tokens, FILE* out FOR_LOGS(, LOG_PARAMS)) 
{
    lang_log_report();
    TOKENS_STRUCT_PTR_CHECK(tokens);

    if (!out)
    {
        error_report(INV_FILE_PTR);
        return 0;
    }

    fprintf(out, "\n<pre>\n");
    fprintf(out, "<div class=\"outline\"  "
                 "style = \"background-color:lightgrey;\" "
                 "style = \"text-align: center;\">");

    struct Token* cur_token = NULL;

    for (int counter = 0; counter < tokens->amount; counter++)
    {
        cur_token = tokens->token_array[counter];

        if (tokens->counter == counter)
            fprintf(out, "<b>");

        fprintf(out, "\n\n Token <%p> Token type: ", cur_token);

        switch(cur_token->type)
        {
            case CONST:
            {
                fprintf(out, "CONSTANT "); 
                fprintf(out, "Data: %g\n\n", cur_token->data.constant);
                break;
            }    

            case ID:
            {
                fprintf(out, "IDENTIFICATOR ");
                fprintf(out, "id hash: %ld\n\n", cur_token->data.id_hash); 
                break;
            }       

            case KEY_WORD:
            {
                fprintf(out, "KEY WORD "); 

                for (int i = 0; i < Key_words_number; i++) 

                    if (cur_token->data.key_word_code == Key_words[i].key_word_code)
                    {
                        fprintf(out, "data: %s ", Key_words[i].name);
                        fprintf(out, "meaning: %s\n\n", Key_words[i].meaning);
                        break;
                    } 

                break;
            } 

            case CMP_SIGN:
            {
                fprintf(out, "COMPARISON SIGN ");

                for (int i = 0; i < Std_cmp_signs_number; i++)
                    if (cur_token->data.cmp_sign_code == Std_cmp_signs[i].code)
                    {
                        fprintf(out, "data: %s\n\n", Std_cmp_signs[i].name);
                        break;
                    }

                break;
            }

            case CALC_FUNC:
            {
                fprintf(out, "CALCULATING FUNCTION ");

                for (int i = 0; i < Calc_functions_number; i++)
                    if (cur_token->data.calc_func_code == Calc_functions[i].code)
                    {
                        fprintf(out, "data: %s\n\n", Calc_functions[i].name);
                        break;
                    }
                    
                break;
            }

            case STD_FUNC:
            {
                fprintf(out, "STANDART FUNCTION ");

                for (int i = 0; i < Std_functions_number; i++)
                    if (cur_token->data.std_func_code == Std_functions[i].code)
                    {
                        fprintf(out, "data: %s\n\n", Std_functions[i].name);
                        break;
                    }

                break;
            }       

            case OPER:
            {
                fprintf(out, "operand data: %c\n\n", cur_token->data.operand);
                break;
            }

            default:
            {
                fprintf(out, "INVALID DATA TYPE ");
                break;
            }
        }

        if (tokens->counter == counter)
            fprintf(out, "</b>");

        fflush(logs_file);
    }

    fprintf(out, "</div> </pre>\n\n");
    fflush(out);
    
    return 0;
}