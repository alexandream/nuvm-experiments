#ifndef __NUVM__TYPE_INFO_H__
#define __NUVM__TYPE_INFO_H__

#include "util/types.h"
#include "error.h"

typedef struct NType NType;
typedef struct NTypeRegistry NTypeRegistry;

struct NType {
	const char* name;
};


void
n_type_init(NType* self, const char* name);

NTypeRegistry*
n_type_registry_new(NError*);

void
n_type_registry_destroy(NTypeRegistry*);


int32_t
n_type_registry_add_type(NTypeRegistry*, NType*, NError*);

int32_t
n_type_registry_count_types(NTypeRegistry*);

NType*
n_type_registry_find_type(NTypeRegistry*, const char* name, int32_t* out_id);

#endif
