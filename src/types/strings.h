#ifndef __NUVM__TYPES__STRING_H__
#define __NUVM__TYPES__STRING_H__

#include "../util/common.h"
#include "../nuvm.h"
#include "../value.h"

uint32_t NUVM_STRING_T_TYPE();

typedef struct nuvm_string_t nuvm_string_t;

nuvm_string_t* nuvm_new_string(const char*);

const char* nuvm_string_get_repr(nuvm_string_t*);
bool nuvm_string_is_equal(nuvm_string_t*, nuvm_string_t*);
uint32_t nuvm_string_length(nuvm_string_t*);

void nuvm_string_t_init();

#endif
