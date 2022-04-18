#pragma once

#include "../lang_common/lang_common.h"
#include "../lang_common/lang_config.h"

//===================================================================

#define BUF (buffer->buffer + buffer->pos)
#define POS (buffer->pos)

//===================================================================

#define ADD_OPERAND_TOKEN(tokens, operand){                         \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_operand_token(new_token, operand);            \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_ID_TOKEN(tokens, id_hash) {                             \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_id_token(new_token, id_hash);                 \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_CONSTANT_TOKEN(tokens, constant){                       \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_constant_token(new_token, constant);          \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_KEY_WORD_TOKEN(tokens, code) {                          \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_key_word_token(new_token, code);              \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_CMP_SIGN_TOKEN(tokens, code){                           \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_cmp_sign_token(new_token, code);              \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_CALC_FUNC_TOKEN(tokens, code){                          \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_calc_func_token(new_token, code);             \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_STD_FUNC_TOKEN(tokens, code){                           \
                                                                    \
    do                                                              \
    {                                                               \
        Token* new_token = token_allocate();                        \
        if (!new_token)                                             \
        {                                                           \
            error_report(CANNOT_ALLOCATE_MEM);                      \
            return -1;                                              \
        }                                                           \
                                                                    \
        tokens->token_array[tokens->amount] = new_token;            \
                                                                    \
        int ret = add_std_func_token(new_token, code);              \
        tokens->amount++;                                           \
        RETURN_CHECK(ret);                                          \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#define IS_NUMBER(symb) ('0' <= symb && symb <= '9')

#define IS_COMPARISON_SYMB(symb) (symb == '<' || symb == '>' || symb == '=' || symb == '#')

#define IS_ALPHA(symb) (('a' <= symb && symb <= 'z') || ('A' <= symb && symb <= 'Z') || symb == '_')

#define IS_BLANK(symb) (isblank(symb) || symb == '\n');

#define NO_TOKEN -2

//===================================================================

#define SYNTAX_ERROR(buffer) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        error_report(SYNTAX_ERR);                                   \
        buffer_dump(buffer);                                        \
    } while(0);                                                     \
}

//===================================================================

#define IS_OPERAND(symb) (symb == ADD \
                       || symb == SUB \
                       || symb == MUL \
                       || symb == DIV \
                       || symb == POW \
                       || symb == DEN) 

//===================================================================

#define TOKENS_STRUCT_PTR_CHECK(tokens) {                           \
                                                                    \
    do                                                              \
    {                                                               \
        if (!tokens)                                                \
        {                                                           \
            error_report(INV_TOKENS_STRUCT_PTR);                    \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define TOKEN_PTR_CHECK(token){                                     \
                                                                    \
    do                                                              \
    {                                                               \
        if (!token)                                                 \
        {                                                           \
            error_report(INV_TOKEN_PTR);                            \
            return -1;                                              \
        }                                                           \
    } while(0);                                                     \
}

//===================================================================

int _tokens_dump(Tokens* tokens, FILE* out FOR_LOGS(, LOG_PARAMS));

int _scan_id(Buffer_struct* buffer, char* id_buf FOR_LOGS(, LOG_PARAMS));

int _token_array_resize(struct Tokens* tokens FOR_LOGS(, LOG_PARAMS));

struct Tokens* _lexic_analysis(Buffer_struct* buffer FOR_LOGS(, LOG_PARAMS));

int _read_token(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS));

struct Tokens* _tokens_struct_allocate(FOR_LOGS(LOG_PARAMS));

struct Token* _token_allocate( FOR_LOGS(LOG_PARAMS));

int _read_id(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _read_operand(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _read_comparison_sign(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _read_number(Buffer_struct* buffer, Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _add_operand_token(Token* token, char operand FOR_LOGS(, LOG_PARAMS));

int _add_constant_token(Token* token, double constant FOR_LOGS(, LOG_PARAMS));

int _add_key_word_token(Token* token, int code FOR_LOGS(, LOG_PARAMS));

int _add_cmp_sign_token(Token* token, int code FOR_LOGS(, LOG_PARAMS));

int _add_calc_func_token(Token* token, int code FOR_LOGS(, LOG_PARAMS));

int _add_std_func_token(Token* token, int code FOR_LOGS(, LOG_PARAMS));

int _add_id_token(Token* token, int64_t hash FOR_LOGS(, LOG_PARAMS));

int _key_word_code(const char* id FOR_LOGS(, LOG_PARAMS));

int _calc_func_code(const char* name FOR_LOGS(, LOG_PARAMS));

int _std_func_code(const char* name FOR_LOGS(, LOG_PARAMS));

int _cmp_sign_code(const char* id FOR_LOGS(, LOG_PARAMS));

double _constant_value(const char* name FOR_LOGS(, LOG_PARAMS));

int _skip_blank_symb(Buffer_struct* buffer FOR_LOGS(, LOG_PARAMS));

int _free_tokens(Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _delete_comments(Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _find_cmnt_close_token(Tokens* tokens FOR_LOGS(, LOG_PARAMS));

int _find_cmnt_open_token(Tokens* tokens FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define find_cmnt_close_token(tokens) \
       _find_cmnt_close_token(tokens FOR_LOGS(, LOG_ARGS))

#define find_cmnt_open_token(tokens) \
       _find_cmnt_open_token(tokens FOR_LOGS(, LOG_ARGS))

#define delete_comments(tokens) \
       _delete_comments(tokens FOR_LOGS(, LOG_ARGS))

#define free_tokens(tokens) \
       _free_tokens(tokens FOR_LOGS(, LOG_ARGS))

#define tokens_dump(tokens, out) \
       _tokens_dump(tokens, out FOR_LOGS(, LOG_ARGS))

#define scan_id(buffer, id_buf) \
       _scan_id(buffer, id_buf FOR_LOGS(, LOG_ARGS))

#define add_id_token(token, id_hash) \
       _add_id_token(token, id_hash FOR_LOGS(, LOG_ARGS))

#define add_operand_token(token, id_hash) \
       _add_operand_token(token, id_hash FOR_LOGS(, LOG_ARGS))

#define token_array_resize(tokens) \
       _token_array_resize(tokens FOR_LOGS(, LOG_ARGS))

#define add_constant_token(token, constant) \
       _add_constant_token(token, constant FOR_LOGS(, LOG_ARGS))

#define add_key_word_token(token, code) \
       _add_key_word_token(token, code FOR_LOGS(, LOG_ARGS)) 

#define add_cmp_sign_token(token, code) \
       _add_cmp_sign_token(token, code FOR_LOGS(, LOG_ARGS)) 

#define add_calc_func_token(token, code) \
       _add_calc_func_token(token, code FOR_LOGS(, LOG_ARGS)) 

#define add_std_func_token(token, code) \
       _add_std_func_token(token, code FOR_LOGS(, LOG_ARGS))

#define read_id(buffer, tokens) \
       _read_id(buffer, tokens FOR_LOGS(, LOG_ARGS))

#define read_comparison_sign(buffer, tokens) \
       _read_comparison_sign(buffer, tokens FOR_LOGS(, LOG_ARGS))

#define read_operand(buffer, tokens) \
       _read_operand(buffer, tokens FOR_LOGS(, LOG_ARGS))   

#define read_number(buffer, tokens) \
       _read_number(buffer, tokens FOR_LOGS(, LOG_ARGS))  

#define read_token(buffer, tokens) \
       _read_token(buffer, tokens FOR_LOGS(, LOG_ARGS))

#define tokens_struct_allocate() \
       _tokens_struct_allocate( FOR_LOGS(LOG_ARGS))

#define token_allocate() \
       _token_allocate( FOR_LOGS(LOG_ARGS))

#define lexic_analysis(buffer) \
       _lexic_analysis(buffer FOR_LOGS(, LOG_ARGS))

#define constant_value(name) \
       _constant_value(name FOR_LOGS(, LOG_ARGS))

#define calc_func_code(name) \
       _calc_func_code(name FOR_LOGS(, LOG_ARGS))

#define std_func_code(name) \
       _std_func_code(name FOR_LOGS(, LOG_ARGS))

#define key_word_code(name) \
       _key_word_code(name FOR_LOGS(, LOG_ARGS))

#define cmp_sign_code(name) \
       _cmp_sign_code(name FOR_LOGS(, LOG_ARGS))

#define skip_blank_symb(buffer) \
       _skip_blank_symb(buffer FOR_LOGS(, LOG_ARGS))
