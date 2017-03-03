#include <stdio.h>

#include "evaluator.h"

#include "../common/opcodes.h"
#include "../common/instruction-decoders.h"

static
NErrorType INDEX_OO_BOUNDS =  { "nuvm.evaluator.IndexOutOfBounds", NULL };

static
NErrorType FIXNUM_OVERFLOW = { "nuvm.math.Overflow", NULL };

static
NErrorType DIV_BY_ZERO = { "nuvm.math.DivisionByZero", NULL };

static int
do_op_add(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_sub(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_mul(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_div(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_eq(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_neq(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_lt(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_lte(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_gt(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_gte(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_load4(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_load16(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_load_bool(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_not(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_and(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_or(NEvaluator *self, NInstructionWord *code, NError *error);

static int
do_op_xor(NEvaluator *self, NInstructionWord *code, NError *error);



int
ni_init_evaluator(void) {
    NError error = n_error_ok();

    if (ni_init_errors() < 0) {
        return -1;
    }
    if (ni_init_values() < 0) {
        return -3;
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
    n_register_error_type(&DIV_BY_ZERO, &error);
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
        case N_OP_MUL:
            self->pc += do_op_mul(self, words, error);
            break;
        case N_OP_DIV:
            self->pc += do_op_div(self, words, error);
            break;
        case N_OP_EQ:
            self->pc += do_op_eq(self, words, error);
            break;
        case N_OP_NEQ:
            self->pc += do_op_neq(self, words, error);
            break;
        case N_OP_LT:
            self->pc += do_op_lt(self, words, error);
            break;
        case N_OP_LTE:
            self->pc += do_op_lte(self, words, error);
            break;
        case N_OP_GT:
            self->pc += do_op_gt(self, words, error);
            break;
        case N_OP_GTE:
            self->pc += do_op_gte(self, words, error);
            break;
        case N_OP_LOAD_INT4:
            self->pc += do_op_load4(self, words, error);
            break;
        case N_OP_LOAD_INT16:
            self->pc += do_op_load16(self, words, error);
            break;
        case N_OP_LOAD_BOOL:
            self->pc += do_op_load_bool(self, words, error);
            break;
        case N_OP_NOT:
            self->pc += do_op_not(self, words, error);
            break;
        case N_OP_AND:
            self->pc += do_op_and(self, words, error);
            break;
        case N_OP_OR:
            self->pc += do_op_or(self, words, error);
            break;
        case N_OP_XOR:
            self->pc += do_op_xor(self, words, error);
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
        if (!n_is_ok(error)) {
            self->halted = 1;
        }
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
multiplication_would_overflow(NFixnum left, NFixnum right) {
    int same_sign = !((left < 0) ^ (right < 0));
    /* We can't have overflow if one of the operands is zero for the
     * result would then be zero. */
    if (left == 0 || right == 0) return 0;
    if (same_sign) {
        return (left < 0) ? left < (N_FIXNUM_MAX / right)
                          : left > (N_FIXNUM_MAX / right);
    }
    else {
        /* if right == -1, the division would overflow, but then, we know
         * the multiplication wouldn't overflow because left would then
         * have to be positive, and any positive number can be made
         * negative without overflowing. */
        return right == -1 || left < N_FIXNUM_MIN / right;
    }
}


static int
division_would_overflow(NFixnum left, NFixnum right) {
    return left == N_FIXNUM_MIN && right == -1;
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
        n_set_error(error, &FIXNUM_OVERFLOW, "The subtraction would overflow.",
                    NULL, NULL);
        return 0;
    }
    set_register(self, dest, n_wrap_fixnum(num1 - num2), error);
    return increment;
}


static int
do_op_mul(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    NFixnum num1, num2;
    int increment = n_decode_op_mul(code, &dest, &arg1, &arg2);

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

    if (multiplication_would_overflow(num1, num2)) {
        n_set_error(error, &FIXNUM_OVERFLOW,
                    "The multiplication would overflow.", NULL, NULL);
        return 0;
    }
    set_register(self, dest, n_wrap_fixnum(num1 * num2), error);
    return increment;
}


static int
do_op_div(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    NFixnum num1, num2;
    int increment = n_decode_op_div(code, &dest, &arg1, &arg2);

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

    if (num2 == 0) {
        n_set_error(error, &DIV_BY_ZERO, "Division by zero.", NULL, NULL);
        return 0;
    }

    if (division_would_overflow(num1, num2)) {
        n_set_error(error, &FIXNUM_OVERFLOW, "The division would overflow.",
                    NULL, NULL);
        return 0;
    }
    set_register(self, dest, n_wrap_fixnum(num1 / num2), error);
    return increment;
}


static int
do_op_eq(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    result = n_wrap_boolean(n_eq_values(val1, val2));
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_neq(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    result = n_wrap_boolean(!n_eq_values(val1, val2));
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_lt(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    NFixnum num1, num2;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

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
    result = n_wrap_boolean(num1 < num2);
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_lte(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    NFixnum num1, num2;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

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
    result = n_wrap_boolean(num1 <= num2);
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_gt(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    NFixnum num1, num2;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

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
    result = n_wrap_boolean(num1 > num2);
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_gte(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2, result;
    NFixnum num1, num2;
    int increment = n_decode_op_eq(code, &dest, &arg1, &arg2);

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
    result = n_wrap_boolean(num1 >= num2);
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_load4(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest;
    int8_t value;
    int increment = n_decode_op_load_int4(code, &dest, &value);

    set_register(self, dest, n_wrap_fixnum(value), error);
    return increment;
}


static int
do_op_load16(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest;
    int16_t value;
    int increment = n_decode_op_load_int16(code, &dest, &value);

    set_register(self, dest, n_wrap_fixnum(value), error);
    return increment;
}


static int
do_op_load_bool(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest;
    uint8_t value;
    int increment = n_decode_op_load_bool(code, &dest, &value);

    set_register(self, dest, n_wrap_boolean(value), error);
    return increment;
}


static int
do_op_not(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest;
    uint8_t arg;
    NValue val;
    NValue result;
    int increment = n_decode_op_not(code, &dest, &arg);

    val = n_evaluator_get_register(self, arg, error);
    if (!n_is_ok(error)) {
        return 0;
    }
    if (n_eq_values(val, N_TRUE)) {
        result = N_FALSE;
    }
    else {
        result = N_TRUE;
    }
    set_register(self, dest, result, error);
    return increment;
}


static int
do_op_and(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    int left, right;
    int increment = n_decode_op_and(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    left = n_unwrap_boolean(val1);
    right = n_unwrap_boolean(val2);

    set_register(self, dest, n_wrap_boolean(left & right), error);

    return increment;
}


static int
do_op_or(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    int left, right;
    int increment = n_decode_op_or(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    left = n_unwrap_boolean(val1);
    right = n_unwrap_boolean(val2);

    set_register(self, dest, n_wrap_boolean(left | right), error);

    return increment;
}


static int
do_op_xor(NEvaluator *self, NInstructionWord *code, NError *error) {
    uint8_t dest, arg1, arg2;
    NValue val1, val2;
    int left, right;
    int increment = n_decode_op_or(code, &dest, &arg1, &arg2);

    val1 = n_evaluator_get_register(self, arg1, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    val2 = n_evaluator_get_register(self, arg2, error);
    if (!n_is_ok(error)) {
        return 0;
    }

    left = n_unwrap_boolean(val1);
    right = n_unwrap_boolean(val2);

    set_register(self, dest, n_wrap_boolean(left ^ right), error);

    return increment;
}
