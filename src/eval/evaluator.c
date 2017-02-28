#include <stdio.h>

#include "evaluator.h"
#include "fixnum.h"

#include "../common/opcodes.h"
#include "../common/instruction-decoders.h"

static
NErrorType INDEX_OO_BOUNDS =  { "nuvm.evaluator.IndexOutOfBounds", NULL };

static
NErrorType FIXNUM_OVERFLOW = { "nuvm.math.Overflow", NULL };

static int
do_op_add(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_sub(NEvaluator *self, NInstructionWord *code, NError *error);

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
    n_register_error_type(&FIXNUM_OVERFLOW, &error);
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
    NInstructionWord *words = self->code + self->pc;
    uint8_t opcode = n_decode_opcode(words);
    switch (opcode) {
        case N_OP_NOP:
            self->pc++;
            break;
        case N_OP_HALT:
            self->halted = 1;
            break;
        case N_OP_ADD:
            self->pc += do_op_add(self, words, error);
            break;
		case N_OP_SUB:
			self->pc += do_op_sub(self, words, error);
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
    if (index < self->num_registers) {
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
    self->num_registers = num_registers;
    self->pc = 0;
    self->halted = 0;
}
#endif /* N_TEST */

static void
set_register(NEvaluator *self, uint8_t index, NValue value, NError *error) {
    if (index < self->num_registers) {
        self->registers[index] = value;
    }
    else {
        n_set_error(error, &INDEX_OO_BOUNDS, "The given index is larger "
                    "than the number of addressable registers "
                    "in this evaluator.", NULL, NULL);
        return;

    }

}


static int
addition_would_overflow(NFixnum left, NFixnum right) {
    return (left >= 0) ? right > (N_FIXNUM_MAX - left)
                       : right < (N_FIXNUM_MIN - left);
}


static int
subtraction_would_overflow(NFixnum left, NFixnum right) {
	return (right == N_FIXNUM_MIN) ? left >= 0
		                           : addition_would_overflow(left, -right);
}


static int
do_op_add(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    NFixnum num1, num2;
    int increment = n_decode_op_add(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    num1 = n_unwrap_fixnum(val1);
    num2 = n_unwrap_fixnum(val2);

    if (addition_would_overflow(num1, num2)) {
        n_set_error(error, &FIXNUM_OVERFLOW, "The addition would overflow.",
                    NULL, NULL);
        return 0;
    }
    set_register(self, dest, n_wrap_fixnum(num1 + num2), error);
    return increment;
}


static int
do_op_sub(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    NFixnum num1, num2;
    int increment = n_decode_op_sub(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    num1 = n_unwrap_fixnum(val1);
    num2 = n_unwrap_fixnum(val2);

    if (subtraction_would_overflow(num1, num2)) {
        n_set_error(error, &FIXNUM_OVERFLOW, "The addition would overflow.",
                    NULL, NULL);
        return 0;
    }
    set_register(self, dest, n_wrap_fixnum(num1 - num2), error);
    return increment;
}
