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
nuvm_value_t nuvm_primitive_fixnum_add(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_flonum_add(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_and(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_or(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_string_eq(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_string_len(void*, nuvm_value_t*, uint8_t);
nuvm_value_t nuvm_primitive_is_same(void*, nuvm_value_t*, uint8_t);
#endif
