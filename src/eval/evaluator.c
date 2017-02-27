#include "evaluator.h"
#include "fixnum.h"
int
ni_init_evaluator(void) {
    return 0;
}


int n_evaluator_is_halted(NEvaluator* self) {
    return 0;
}


void n_evaluator_step(NEvaluator *self, NError *error) {
}


void n_evaluator_run(NEvaluator *self, NError *error) {

}


NValue
n_evaluator_get_register(NEvaluator *self, int index, NError *error) {
    return n_wrap_fixnum(0);
}


#ifdef N_TEST

void
nt_construct_evaluator(NEvaluator* self, NInstructionWord* code,
                       size_t code_size, NValue* registers,
                       size_t num_registers) {
    self->pc = 10;
}
#endif /* N_TEST */
