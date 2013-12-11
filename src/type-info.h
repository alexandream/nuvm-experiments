#ifndef __NUVM__TYPE_INFO_H__
#define __NUVM__TYPE_INFO_H__

#include "util/types.h"

typedef struct NType NType;

struct NType {
	uint32_t id;
	const char* name;
};

void 
n_type_init(NType* self, const char* name);

#endif
