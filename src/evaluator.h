#ifndef __NUVM__EVALUATOR_H__
#define __NUVM__EVALUATOR_H__

#include "util/types.h"

#include "error.h"
#include "value.h"

#include "module.h"

typedef struct NEvaluator NEvaluator;

NEvaluator*
n_evaluator_new(NError*);

void
n_evaluator_destroy(NEvaluator*);

NValue
n_evaluator_run(NEvaluator*, NError*);

void
n_evaluator_setup(NEvaluator*, NModule* mod);
#endif

