#ifndef __NUVM__TYPE_INFO_H__
#define __NUVM__TYPE_INFO_H__

#include "util/types.h"
#include "value.h"
#include "error.h"

typedef struct NType NType;
typedef struct NTypeRegistry NTypeRegistry;

struct NType {
	const char* name;

	char* (*to_string)(const NType*, NValue);
};


void
n_type_init(NType* self, const char* name);

NTypeRegistry*
n_type_registry_get_default();

NTypeRegistry*
n_type_registry_new(NError*);

void
n_type_registry_destroy(NTypeRegistry*);


int32_t
n_type_registry_add_type(NTypeRegistry*, NType*, NError*);

int32_t
n_type_registry_count_types(NTypeRegistry*);

NType*
n_type_registry_fetch_type(NTypeRegistry*, int32_t id);

NType*
n_type_registry_find_type(NTypeRegistry*, const char* name, int32_t* out_id);

bool
n_type_registry_has_type(NTypeRegistry*, const char* name);

#endif
