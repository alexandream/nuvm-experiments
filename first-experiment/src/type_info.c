#include <string.h> 
#include "util/common.h"

#include "type_info.h"

#ifndef NUVM_MAX_TYPE_INFO
#define NUVM_MAX_TYPE_INFO 64
#endif


static uint32_t _pool_size = 0;
static nuvm_type_info_t* _pool[NUVM_MAX_TYPE_INFO];

static bool _has_type(const char*, uint32_t*);

uint32_t nuvm_type_info_register(nuvm_type_info_t* info)
{
	assert(!_has_type(info->name, NULL));
	assert(_pool_size < NUVM_MAX_TYPE_INFO);
	uint32_t index = _pool_size++;
	_pool[index] = info;
	return index +1;
}

nuvm_type_info_t* nuvm_type_info_load(uint32_t id) {
	assert(id < _pool_size);
	assert(id > 0);
	return _pool[id-1];
}

uint32_t nuvm_type_info_resolve(const char* name) {
	uint32_t index;
	bool found = _has_type(name, &index);
	
	assert(found);

	return index +1;
}

static bool _has_type(const char* name, uint32_t* index) {
	uint32_t i;
	bool found = false;
	for (i=0; !found && i < _pool_size; i++) {
		if (strcmp(_pool[i]->name, name) == 0) {
			found = true;
		}
	}
	if (index != NULL) {
		*index = (found ? i : 0);
	}
		
	return found;
}

