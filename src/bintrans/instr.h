#pragma once 

#include "bintrans_conf.h"

//===============================================

#ifndef ENTITY_ADD_NAME_STR

    #define DEF_INSTR(Num, Name, ...)                       \
                                                            \
        static const unsigned char Name[] = { __VA_ARGS__ };\
                                                            \
        static const unsigned int Name##_size =             \
            sizeof(Name) / sizeof(unsigned char);           

#else 

    #define DEF_INSTR(Num, Name, ...)                       \
                                                            \
        static const unsigned char Name[] = { __VA_ARGS__ };\
                                                            \
        static const unsigned int Name##_size =             \
            sizeof(Name) / sizeof(unsigned char);           \
                                                            \
        static const char Name##_str[] = "#Name";                  

#endif 

//-----------------------------------------------

#include "../../text_files/instr.txt"

//-----------------------------------------------

#undef DEF_INSTR

//===============================================

#define DEF_CMD_(args_num, name, code, hash, instr)         \
                                                            \
        name = code,

#define DEF_JMP_(args_num, name, code, hash, instr)         \
                                                            \
        name = code,

//-----------------------------------------------

enum Codes
{
    #include "../../text_files/commands.txt"
    #include "../../text_files/jumps.txt"
};

//-----------------------------------------------

#undef DEF_CMD_
#undef DEF_JMP_

//===============================================

#define DEF_INSTR(Num, Name, ...)                           \
                                                            \
    Name##_type = Num, 

//-----------------------------------------------

enum Entity_types
{
    #include "../../text_files/instr.txt"
};

//-----------------------------------------------

#undef DEF_INSTR