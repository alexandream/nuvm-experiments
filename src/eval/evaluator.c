#include <stdio.h>

#include "evaluator.h"
#include "fixnum.h"

#include "../common/opcodes.h"

static
NErrorType INDEX_OO_BOUNDS =  { "nuvm.evaluator.IndexOutOfBounds", NULL };

static uint8_t
get_current_opcode(NEvaluator *self);


int
ni_init_evaluator(void) {
    NError error = n_error_ok();

    if (ni_init_errors() < 0) {
        return -1;
    }

    n_register_error_type(&INDEX_OO_BOUNDS, &error);
    if (!n_is_ok(&error)) {
        n_destroy_error(&error);
        return -2;
    }
    return 0;
}


int n_evaluator_is_halted(NEvaluator* self) {
    return self->halted;
}


void n_evaluator_step(NEvaluator *self, NError *error) {
    uint8_t opcode = get_current_opcode(self);
    switch (opcode) {
        case N_OP_NOP:
            self->pc++;
            break;
        case N_OP_HALT:
            self->halted = 1;
            break;
        default: {
            self->halted = 1;
            break;
        }
    }
}


void n_evaluator_run(NEvaluator *self, NError *error) {
    while (! self->halted ) {
        n_evaluator_step(self, error);
    }
}


NValue
n_evaluator_get_register(NEvaluator *self, int index, NError *error) {
    if (index < self->code_size) {
        return self->registers[index];
    }
    else {
        n_set_error(error, &INDEX_OO_BOUNDS, "The given index is larger "
                    "than the number of addressable registers "
                    "in this evaluator.", NULL, NULL);
        return n_wrap_fixnum(-1);

    }
}


#ifdef N_TEST
void
nt_construct_evaluator(NEvaluator* self, NInstructionWord* code,
                       int code_size, NValue* registers, int num_registers) {
    self->code = code;
    self->code_size = code_size;
    self->registers = registers;
    self->pc = 0;
    self->halted = 0;
}
#endif /* N_TEST */


static uint8_t
get_current_opcode(NEvaluator *self) {
    return n_decode_opcode((self->code+self->pc));
}
