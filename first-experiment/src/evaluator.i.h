#ifndef __NUVM__EVALUATOR_I_H__
#define __NUVM__EVALUATOR_I_H__

#include "types/procedure.h"

#include "evaluator.h"

struct nuvm_evaluator_t {
	nuvm_procedure_t* current_proc;
	nuvm_module_t* current_module;
	uint32_t code_pointer;
};

uint32_t _nuvm_evaluator_fill_call_args(nuvm_evaluator_t* self,
                                       nuvm_value_t* args,
                                       uint8_t num_args);

#endif
