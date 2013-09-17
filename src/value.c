#include "util/common.h"
#include "value.h"

#include "type_info.h"

nuvm_value_t NUVM_TRUE,
             NUVM_FALSE;
static bool _initialized = false;

static uint32_t _boolean_type_id = 0;
static uint32_t _fixnum_type_id = 0;
static uint32_t _flonum_type_id = 0;

static nuvm_type_info_t _boolean_type_info;
static nuvm_type_info_t _flonum_type_info;
static nuvm_type_info_t _fixnum_type_info;

void nuvm_value_t_init() {
	if (!_initialized) {
		_initialized = true;
		_boolean_type_info.name = "org.nuvm.Boolean";
		_boolean_type_id = nuvm_type_info_register(&_boolean_type_info);
		
		_fixnum_type_info.name = "org.nuvm.Fixnum";
		_fixnum_type_id = nuvm_type_info_register(&_fixnum_type_info);

		_flonum_type_info.name = "org.nuvm.Flonum";
		_flonum_type_id = nuvm_type_info_register(&_flonum_type_info);
	}

	NUVM_TRUE = nuvm_wrap_pointer(&NUVM_TRUE);
	NUVM_FALSE = nuvm_wrap_pointer(&NUVM_FALSE);

}

uint32_t NUVM_FIXNUM_T_TYPE() {
	return _fixnum_type_id;
}

uint32_t NUVM_FLONUM_T_TYPE() {
	return _flonum_type_id;
}

uint32_t NUVM_BOOLEAN_T_TYPE() {
	return _boolean_type_id;
}
