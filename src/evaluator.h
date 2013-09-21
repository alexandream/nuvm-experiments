#ifndef __NUVM__EVALUATOR_H__
#define __NUVM__EVALUATOR_H__

#include "util/common.h"
#include "module.h"


typedef struct nuvm_evaluator_t nuvm_evaluator_t;

nuvm_evaluator_t* nuvm_new_evaluator();
void nuvm_destroy_evaluator(nuvm_evaluator_t* self);

void nuvm_evaluator_setup(nuvm_evaluator_t* self, nuvm_module_t* module);

bool nuvm_evaluator_step(nuvm_evaluator_t* self, nuvm_value_t* result);
nuvm_value_t nuvm_evaluator_run(nuvm_evaluator_t* self);


#endif

