#ifndef N_E_EVALUATOR
#define N_E_EVALUATOR

#include <stdlib.h>

#
#include "../common/instruction-formats.h"
#include "value.h"

typedef struct NEvaluator NEvaluator;


struct NEvaluator {
    NInstructionWord *code;
    int code_size;
    NValue *registers;
    int num_registers;

    int pc;
    int halted;
};

int
ni_init_evaluator(void);


int n_evaluator_is_halted(NEvaluator* self);

void n_evaluator_step(NEvaluator *self, NError *error);

void n_evaluator_run(NEvaluator *self, NError *error);

NValue
n_evaluator_get_register(NEvaluator *self, int index, NError *error);

#ifdef N_TEST
void
nt_construct_evaluator(NEvaluator* self, NInstructionWord* code,
                       int code_size, NValue* registers, int num_registers);

#endif /* N_TEST */

#endif /* N_E_EVALUATOR */
