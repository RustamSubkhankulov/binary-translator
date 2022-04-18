#pragma once

//===================================================================

#define DEF_ERR(code, name, string) 								 \
																	 \
	name  = code,													 \

//===================================================================

enum errors {

	#include "../include/errors.txt"
};

#undef DEF_ERR

