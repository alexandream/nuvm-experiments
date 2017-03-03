#ifndef __NUVM__TYPE_INFO_H__
#define __NUVM__TYPE_INFO_H__

#include "util/common.h"

#include "value.h"

typedef struct nuvm_type_info_t {
	const char*  name;
} nuvm_type_info_t;

uint32_t nuvm_type_info_register(nuvm_type_info_t* info);
uint32_t nuvm_type_info_resolve(const char* name);
nuvm_type_info_t* nuvm_type_info_load(uint32_t id);

#endif

