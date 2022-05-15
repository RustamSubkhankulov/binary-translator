#pragma once 

//===============================================

#include "../global_conf.h"

//===============================================

//#define BINTRANS_LISTING

#ifdef BINTRANS_LISTING

    static const char* Listing_filename = "listing/listing.txt";

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

static const unsigned int Entities_initial_cap = 16;

static const unsigned int Ram_size = 2048; 

static const unsigned int Initial_jumps_cap = 8;

static const unsigned int Initial_pos_cap = 16;