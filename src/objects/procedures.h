#ifndef __NUVM__OBJECTS__PRCEDURES_H_
#define __NUVM__OBJECTS__PRCEDURES_H_

#include "../error.h"
#include "../value.h"
#include "../module.h"

typedef struct NProcedure NProcedure;

void
n_init_procedures();

bool
n_is_procedure(NValue val);

NProcedure*
n_procedure_new(NModule* mod,
                uint32_t entry,
                uint8_t nlocals,
                NError* error);

uint8_t
n_procedure_count_locals(NProcedure*);

uint32_t
n_procedure_get_entry_point(NProcedure*);

NModule*
n_procedure_get_module(NProcedure*);

#endif
