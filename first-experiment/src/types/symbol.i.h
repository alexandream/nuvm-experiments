#ifndef __NUVM__TYPES__SYMBOL_I_H__
#define __NUVM__TYPES__SYMBOL_I_H__

#include "symbol.h"

struct nuvm_symbol_t {
	nuvm_object_t parent;
	const char* string;
	uint32_t size;
};


#endif
