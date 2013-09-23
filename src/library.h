#ifndef __NUVM_LIBRARY_H__
#define __NUVM_LIBRARY_H__

#include "value.h"

nuvm_value_t nuvm_primitive_successor(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_predecessor(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_negate(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_is_positive(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_is_negative(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_is_zero(void *, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_not(void *, nuvm_value_t*, uint8_t);

#endif
