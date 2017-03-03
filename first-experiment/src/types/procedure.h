#ifndef __NUVM__TYPES__PROCEDURE_H__
#define __NUVM__TYPES__PROCEDURE_H__

#include "../util/common.h"
#include "../nuvm.h"
#include "../value.h"
#include "../module.h"

uint32_t NUVM_PROCEDURE_T_TYPE();

typedef struct nuvm_procedure_t nuvm_procedure_t;

nuvm_procedure_t* nuvm_new_procedure(nuvm_module_t* module,
                                     uint32_t entry_point,
                                     uint8_t num_locals);

nuvm_module_t* nuvm_procedure_get_module(nuvm_procedure_t*);
uint32_t  nuvm_procedure_get_entry_point(nuvm_procedure_t*);
uint8_t    nuvm_procedure_get_num_locals(nuvm_procedure_t*);

nuvm_value_t nuvm_procedure_get_local(nuvm_procedure_t*, uint8_t);
void nuvm_procedure_set_local(nuvm_procedure_t*, uint8_t, nuvm_value_t);

void nuvm_procedure_t_init();
size_t nuvm_procedure_t_size();


#endif
