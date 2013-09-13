#ifndef __NUVM__TYPES__PRIMITIVE_H__
#define __NUVM__TYPES__PRIMITIVE_H__

#include "../util/common.h"
#include "../nuvm.h"
#include "../value.h"

uint32_t NUVM_PRIMITIVE_T_TYPE();

typedef struct nuvm_primitive_t nuvm_primitive_t;

typedef nuvm_value_t(*nuvm_native_function_t)(void*);

nuvm_primitive_t* nuvm_new_primitive(nuvm_native_function_t, void*);
nuvm_value_t nuvm_primitive_call(nuvm_primitive_t* self);

void nuvm_primitive_t_init();
size_t nuvm_primitive_t_size();


#endif
