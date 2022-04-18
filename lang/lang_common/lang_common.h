#pragma once

#include "lang_config.h"
#include "../logs/errors_and_logs.h"
#include "../text_processing/text_processing.h"
#include "../proc/assembler/video_ram_config.h"

//===================================================================

struct Calc_function
{
    const char* name;
    int code;
    int64_t hash;
};

//-------------------------------------------------------------------

typedef struct Calc_function Cmp_sign;

struct Std_function
{
    const char* name;
    int code;
    int64_t hash;
    const char* meaning;
};

//-------------------------------------------------------------------

struct Named_constant
{
    const char* name;
    double value;
    int64_t hash;
};

//===================================================================

enum Calc_functions_codes
{
    SIN    = 100,
    COS    = 101,
    SQRT   = 102,
    TG     = 103,
    CTG    = 104,
    LN     = 105,
    ARCCOS = 106,
    ARCSIN = 107,
    ARCTG  = 108,
    ARCCTG = 109,
    SQR    = 110
};

//-------------------------------------------------------------------

enum Std_functions_codes
{
    PRINT = 991,
    SCAN  = 992,
    COPY  = 993,
    DRAW  = 994
};

//-------------------------------------------------------------------

enum Std_cmp_signs_codes
{
    EQ     = 1001,
    NEQ    = 1002,
    MEQ    = 1003,
    LEQ    = 1004,
    MORE   = 1005,
    LESS   = 1006
};

//===================================================================

const static struct Calc_function Calc_functions[] = 
{
    {"sin"   , SIN   , 0}, //
    {"cos"   , COS   , 0}, //
    {"sqrt"  , SQRT  , 0},
    {"tg"    , TG    , 0}, //
    {"ctg"   , CTG   , 0},
    {"ln"    , LN    , 0}, // 
    {"arccos", ARCCOS, 0}, 
    {"arcsin", ARCSIN, 0}, //
    {"arctg" , ARCTG , 0}, //
    {"atcctg", ARCCTG, 0},
    {"sqr"   , SQR   , 0}
};

//-------------------------------------------------------------------

const static struct Std_function Std_functions[] =
{
    {"crucio"    , SCAN , 0, "scan"          },
    {"protego"   , PRINT, 0, "print"         },
    {"expulso"   , COPY , 0, "video ram copy"},
    {"morsmordre", DRAW , 0, "video ram draw"}
};

//-------------------------------------------------------------------

const static struct Named_constant Std_constants[] =
{
    {"pi",         3.142857                  , 0},
    {"e" ,         2.718282                  , 0},
    {"DEADINSIDE", 993                       , 0},
    {"VLEN"      , FRAME_LENGTH              , 0},
    {"VWID"      , FRAME_WIDTH               , 0},
    {"FSIZE"     , FRAME_WIDTH * FRAME_LENGTH, 0}
};

//-------------------------------------------------------------------

const static Cmp_sign Std_cmp_signs[] = 
{
    {"==", EQ,   0},
    {"#=", NEQ,  0},
    {">=", MEQ,  0},
    {"<=", LEQ,  0},
    {">" , MORE, 0},
    {"<" , LESS, 0}
};

//-------------------------------------------------------------------

const int Std_constants_number = sizeof(Std_constants) / sizeof(struct Named_constant);

const int Std_cmp_signs_number = sizeof(Std_cmp_signs) / sizeof(Cmp_sign);

const int Calc_functions_number = sizeof(Calc_functions) / sizeof(struct Calc_function);

const int Std_functions_number = sizeof(Std_functions) / sizeof(struct Std_function);

//===================================================================

enum Operands 
{
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    POW = '^',
    DEN = '!'
};

//===================================================================

struct Tokens
{
    struct Token** token_array;

    int amount;
    int capacity;
    int counter;
};

//-------------------------------------------------------------------

union Token_data
{
    double constant;
    int64_t id_hash;
    int key_word_code;
    int cmp_sign_code;
    int calc_func_code;
    int std_func_code;
    char operand;
};

//-------------------------------------------------------------------

struct Token
{
    int type;

    union Token_data data;
};

//-------------------------------------------------------------------

enum token_types
{
    CONST     = 201,
    ID        = 202,
    KEY_WORD  = 203,
    CMP_SIGN  = 204,
    CALC_FUNC = 206,
    OPER      = 207,
    STD_FUNC  = 208
};

//===================================================================

struct Key_word
{
    const char* name;
    int key_word_code;
    const char* meaning;
    int64_t hash;
};

//-------------------------------------------------------------------

enum Key_word_codes
{
    START      = 97,
    END        = 98,
    EOL        = 100,
    LABEL      = 101,
    GOTO       = 102,
    RET        = 103,
    DO         = 104,
    WHILE      = 105,
  //BREAK      = 106,
  //CONT       = 107,
    IF         = 108,
    ELSE       = 109,
    BR_OPEN    =  110,
    BR_CLOSE   = 111,
    ASS        = 112,
    DECL       = 113,
    DEFN       = 114,
    NEG        = 115,
    FBR_OPEN   = 116,
    FBR_CLOSE  = 117,
    PERM       = 118,
    ARR        = 119,
    SBR_OPEN   = 120,
    SBR_CLOSE  = 121,
    COMMA      = 122,
    CMNT_OPEN  = 123,
    CMNT_CLOSE = 124
};

//-------------------------------------------------------------------

const struct Key_word Key_words[] = 
{
    {"obliviate"        , END       , "end of code"         , 0},
    {"flipendo"         , EOL       , "end of the line"     , 0},
    {"accio"            , GOTO      , "goto"                , 0},
    {"geminio"          , LABEL     , "label"               , 0},
    {"expelliarmus"     , RET       , "return"              , 0},
    {"imperio"          , DO        , "do"                  , 0},
    {"confundo"         , WHILE     , "while"               , 0},
  //{"bombradamaxima"   , BREAK     , "break"               , 0},
  //{"sectumsempra"     , CONT      , "continue"            , 0}, 
    {"reducto"          , IF        , "if"                  , 0},
    {"aguamenti"        , ELSE      , "else"                , 0},
    {"avada"            , BR_OPEN   , "open bracket"        , 0},
    {"cedavra"          , BR_CLOSE  , "close bracket"       , 0},
    {"riddikulus"       , ASS       , "assignment"          , 0},
    {"lumos"            , START     , "start of the code"   , 0},
    {"stupefy"          , DECL      , "declaration"         , 0},
    {"colorvaria"       , DEFN      , "func definition"     , 0},
    {"cursed"           , NEG       , "negative"            , 0},
    {"wingardium"       , FBR_OPEN  , "figure_bracket open" , 0},
    {"leviosa"          , FBR_CLOSE , "fugire bracket close", 0},
    {"ascendio"         , PERM      , "permanent"           , 0},
    {"expecto"          , SBR_OPEN  , "square open bracket" , 0},
    {"patronum"         , SBR_CLOSE , "square close bracket", 0},
    {"nox"              , COMMA     , "comma"               , 0},
    {"alohomora"        , ARR       , "array"               , 0},
    {"petrificus"       , CMNT_OPEN , "comment open"        , 0},
    {"totalus"          , CMNT_CLOSE, "comment close"       , 0}
};

const int Key_words_number = sizeof(Key_words) / sizeof(Key_word);

//aberto opens door
//accio
//aguamenti water
//alohomora opens door
//avadakedavra
//bombrada maxima
//colovaria
//confringo
//confundo
//crucio
//expectopatronum
//expelliarmus
//geminio
//imperio
//lumos
//protego
//reducto
//riddikulus
//sectumsempra
//wingardiumleviosa
//flipendo

//===================================================================

#ifdef LANG_LOGS

    #define FOR_LOGS(...) __VA_ARGS__

    #define lang_log_report() \
            log_report()

#else

    #define lang_log_report() \
            dummy_func()

    #define FOR_LOGS(...)

#endif

#ifndef LANGUAGE_LOGS

    #define lang_log_report() \
            dummy_func()

#endif

//===================================================================

int _code_read_file(Buffer_struct* buffer, const char* filename FOR_LOGS(, LOG_PARAMS));

int _code_read_console(Buffer_struct* buffer FOR_LOGS(, LOG_PARAMS));

int _assemble_and_execute(const char* asm_code FOR_LOGS(, LOG_PARAMS));

//===================================================================

#define code_read_file(buffer, filename) \
       _code_read_file(buffer, filename FOR_LOGS(, LOG_ARGS))

#define code_read_console(buffer) \
       _code_read_console(buffer FOR_LOGS(, LOG_ARGS))

#define assemble_and_execute(asm_code) \
       _assemble_and_execute(asm_code FOR_LOGS(, LOG_ARGS))

//===================================================================

