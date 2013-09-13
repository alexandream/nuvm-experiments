#ifndef __NUVM__EVALUATOR_H__
#define __NUVM__EVALUATOR_H__

#include "util/common.h"
#include "module.h"


typedef struct nuvm_evaluator_t nuvm_evaluator_t;

nuvm_evaluator_t* nuvm_new_evaluator();
void nuvm_destroy_evaluator(nuvm_evaluator_t* self);

nuvm_value_t nuvm_evaluator_run_module(nuvm_evaluator_t* self,
                                       nuvm_module_t* module);


#endif

