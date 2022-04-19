#pragma once 

//===============================================

#include "../global_conf.h"

//===============================================

static const unsigned int Entities_init_cap = 100;

//-----------------------------------------------

#define LISTING

#ifdef LISTING

    static const char* Listing_filename = "text_files/listing.txt";

#endif 

//===============================================

#ifdef LOGS

    // Turns on log reports in binary translator functions

    #define BINTRANS_LOGS

#endif 

//-----------------------------------------------

// Turns on validator for BinaryTranslator structure

#define BINTRANS_STRUCT_VALID_CHECK

//===============================================

// #ifdef BINTRANS_LOGS

//     // Turns on dumps of BinaryTranslator structure

//     #define BINTRANS_DUMPS

// #endif 

//-----------------------------------------------

