#ifndef __NUVM__SYMBOL_POOL_H__
#define __NUVM__SYMBOL_POOL_H__

#include "value.h"
#include "error.h"

void
n_symbol_pool_init();

NValue
n_symbol_pool_get_symbol(const char* contents, NError*);

const char*
n_symbol_pool_fetch(int32_t id);

#endif
