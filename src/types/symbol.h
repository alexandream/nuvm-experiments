#ifndef __NUVM__TYPES__SYMBOL_H__
#define __NUVM__TYPES__SYMBOL_H__

#include "../util/common.h"
#include "../nuvm.h"
#include "../value.h"

uint32_t NUVM_SYMBOL_T_TYPE();

typedef struct nuvm_symbol_t nuvm_symbol_t;

nuvm_symbol_t* nuvm_new_symbol(const char*);

void nuvm_symbol_t_init();

#endif
