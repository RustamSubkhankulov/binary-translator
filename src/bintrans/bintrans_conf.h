#pragma once 

//===============================================

#include "../global_conf.h"

//===============================================

#define BINTRANS_LISTING

#ifdef BINTRANS_LISTING

    static const char* Listing_filename = "listing/listing.txt";

#endif 

//===============================================

#ifdef LOGS

    // Turns on log reports in binary translator functions

    #define BINTRANS_LOGS

#endif 

//-----------------------------------------------

// Places 'int 3' interrupt just before executing translated code

#define DEBUG_EXEC

//-----------------------------------------------

// Turns on validator for BinaryTranslator structure

#define BINTRANS_STRUCT_VALID_CHECK

//===============================================

// #ifdef BINTRANS_LOGS

//     // Turns on dumps of BinaryTranslator structure

//     #define BINTRANS_DUMPS

// #endif 

//-----------------------------------------------

