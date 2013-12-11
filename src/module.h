#ifndef __NUVM__MODULE_H__
#define __NUVM__MODULE_H__

#include "util/types.h"
#include "value.h"
#include "error.h"

typedef struct NModule NModule;

NModule*
n_module_new(uint16_t register_count, uint16_t entry, NError* error);

void
n_module_destroy(NModule*);

NValue
n_module_get_register(NModule*, uint16_t index, NError* error);

void
n_module_set_register(NModule*, uint16_t index, NValue val, NError* error);

#endif

