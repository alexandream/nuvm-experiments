#include <stdlib.h>
#include "../test.h"

#include "common/errors.h"

#include "common/instructions.h"

#include "eval/evaluator.h"


#define CODE_SIZE 16
#define NUM_REGISTERS 16

typedef struct {
    NFixnum val1;
    NFixnum val2;
    NFixnum expected;
} FixnumBinopData;


typedef struct {
    uint8_t dest;
    NFixnum value;
} FixnumLoadData;


static
NInstructionWord CODE[CODE_SIZE];


static
NValue REGISTERS[NUM_REGISTERS];


static
NEvaluator EVAL;

static
NError ERROR;

CONSTRUCTOR(constructor) {
    if (ni_init_evaluator() < 0) {
        ERROR("Can't initialize evaluator module.", NULL);
    }
}


SETUP(setup) {
    int i;
    for (i = 0; i < CODE_SIZE; i++) {
        n_encode_op_nop(&CODE[i]);
    }

    for (i = 0; i < NUM_REGISTERS; i++) {
        REGISTERS[i] = n_wrap_fixnum(0);
    }
    nt_construct_evaluator(&EVAL, CODE, CODE_SIZE, REGISTERS, NUM_REGISTERS);
    ERROR = n_error_ok();
}


TEARDOWN(teardown) {
    n_destroy_error(&ERROR);
}


TEST(op_load_int4_increments_pc_by_1) {
    n_encode_op_load_int4(CODE, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 1));
}


FixnumLoadData load4_array[] = {
    { 0, 7 },
    { 15, -8 },
    { 3, 0 }
};
AtArrayIterator load4_iter = at_static_array_iterator(load4_array);
DD_TEST(op_load4_loads_correct_value, load4_iter, FixnumLoadData, load) {
    uint8_t dest = load->dest;
    NFixnum value = load->value;
    NFixnum result;

    n_encode_op_load_int4(CODE, dest, value);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));
    ASSERT(IS_OK(ERROR));

    ASSERT(EQ_INT(result, value));
}


TEST(op_load_int16_increments_pc_by_2) {
    n_encode_op_load_int16(CODE, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


FixnumLoadData load16_array[] = {
    { 0, 32767 },
    { 15, -32768 },
    { 3, 0 }
};
AtArrayIterator load16_iter = at_static_array_iterator(load16_array);
DD_TEST(op_load16_loads_correct_value, load16_iter, FixnumLoadData, load) {
    uint8_t dest = load->dest;
    NFixnum value = load->value;
    NFixnum result;

    n_encode_op_load_int16(CODE, dest, value);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));
    ASSERT(IS_OK(ERROR));

    ASSERT(EQ_INT(result, value));
}


TEST(op_add_increments_pc_by_2) {
    n_encode_op_add(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


FixnumBinopData add_array[] = {
    { 123, 321, 444 },
    { N_FIXNUM_MAX, 0, N_FIXNUM_MAX },
    { N_FIXNUM_MAX, N_FIXNUM_MIN, -1 },
    { 0, 0, 0 },
    { N_FIXNUM_MIN, 0, N_FIXNUM_MIN },
    { N_FIXNUM_MIN, N_FIXNUM_MAX, -1 }
};
AtArrayIterator add_iter = at_static_array_iterator(add_array);
DD_TEST(op_add_correctly_adds, add_iter, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;
    NFixnum expected = binop->expected;
    NFixnum result;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_add(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));

    ASSERT(EQ_INT(result, expected));
}


FixnumBinopData add_overflow_array[] = {
    { N_FIXNUM_MAX, 1, 0},
    { N_FIXNUM_MIN, -1, 0 }
};
AtArrayIterator add_overflow_i = at_static_array_iterator(add_overflow_array);
DD_TEST(op_add_detects_overflow, add_overflow_i, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;

    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_add(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_ERROR(ERROR, "nuvm.math.Overflow"));
}


TEST(op_sub_increments_pc_by_2) {
    n_encode_op_sub(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


FixnumBinopData sub_array[] = {
    { 123, 321, -198 },
    { 0 , N_FIXNUM_MAX, - N_FIXNUM_MAX },
    { N_FIXNUM_MAX, N_FIXNUM_MAX, 0 },
    { 0, 0, 0 },
    { N_FIXNUM_MIN, 0, N_FIXNUM_MIN }
};
AtArrayIterator sub_iter = at_static_array_iterator(sub_array);
DD_TEST(op_sub_correctly_subtracts, sub_iter, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;
    NFixnum expected = binop->expected;
    NFixnum result;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_sub(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));

    ASSERT(EQ_INT(result, expected));
}


FixnumBinopData sub_overflow_array[] = {
    { N_FIXNUM_MAX, -1, 0},
    { 0, N_FIXNUM_MIN, 0},
    { 123, N_FIXNUM_MIN, 0},
    { N_FIXNUM_MIN, 1, 0 }
};
AtArrayIterator sub_overflow_i = at_static_array_iterator(sub_overflow_array);
DD_TEST(op_sub_detects_overflow, sub_overflow_i, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;

    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_sub(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_ERROR(ERROR, "nuvm.math.Overflow"));
}


TEST(op_mul_increments_pc_by_2) {
    n_encode_op_mul(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


FixnumBinopData mul_array[] = {
    { 0, 0, 0 },
    { 0, N_FIXNUM_MAX, 0 },
    { 0, N_FIXNUM_MIN, 0 },
    { N_FIXNUM_MAX, 0, 0 },
    { N_FIXNUM_MIN, 0, 0 },
    { 1, N_FIXNUM_MAX, N_FIXNUM_MAX },
    { -1, N_FIXNUM_MAX, -N_FIXNUM_MAX },
    { 1, N_FIXNUM_MIN, N_FIXNUM_MIN }
};
AtArrayIterator mul_iter = at_static_array_iterator(mul_array);
DD_TEST(op_mul_correctly_multiplies, mul_iter, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;
    NFixnum expected = binop->expected;
    NFixnum result;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_mul(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));

    ASSERT(EQ_INT(result, expected));
}


FixnumBinopData mul_overflow_array[] = {
    { (N_FIXNUM_MAX/2) + 1, 2, 0},
    { -1, N_FIXNUM_MIN, 0},
    { N_FIXNUM_MIN, -1, 0},
};
AtArrayIterator mul_overflow_i = at_static_array_iterator(mul_overflow_array);
DD_TEST(op_mul_detects_overflow, mul_overflow_i, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;

    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_mul(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_ERROR(ERROR, "nuvm.math.Overflow"));
}


TEST(op_div_increments_pc_by_2) {
    n_encode_op_div(CODE, 0, 0, 0);
    REGISTERS[0] = n_wrap_fixnum(1);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


FixnumBinopData div_array[] = {
    { 0, N_FIXNUM_MAX, 0 },
    { 0, N_FIXNUM_MIN, 0 },
    { N_FIXNUM_MAX,  1,  N_FIXNUM_MAX },
    { N_FIXNUM_MAX, -1, -N_FIXNUM_MAX },
    { 1, N_FIXNUM_MIN, 0 },
    { 256, 2, 128 },
};
AtArrayIterator div_iter = at_static_array_iterator(div_array);
DD_TEST(op_div_correctly_divides, div_iter, FixnumBinopData, binop) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NFixnum val1 = binop->val1;
    NFixnum val2 = binop->val2;
    NFixnum expected = binop->expected;
    NFixnum result;

    REGISTERS[arg1] = n_wrap_fixnum(val1);
    REGISTERS[arg2] = n_wrap_fixnum(val2);

    n_encode_op_div(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, dest, &ERROR));

    ASSERT(EQ_INT(result, expected));
}


TEST(op_div_detects_overflow) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;

    REGISTERS[arg1] = n_wrap_fixnum(N_FIXNUM_MIN);
    REGISTERS[arg2] = n_wrap_fixnum(-1);

    n_encode_op_div(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_ERROR(ERROR, "nuvm.math.Overflow"));
}


TEST(op_div_detects_div_by_zero) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;

    REGISTERS[arg1] = n_wrap_fixnum(123);
    REGISTERS[arg2] = n_wrap_fixnum(0);

    n_encode_op_div(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_ERROR(ERROR, "nuvm.math.DivisionByZero"));
}


AtTest* tests[] = {
    &op_load_int4_increments_pc_by_1,
    &op_load4_loads_correct_value,
    &op_load_int16_increments_pc_by_2,
    &op_load16_loads_correct_value,

    &op_add_increments_pc_by_2,
    &op_add_correctly_adds,
    &op_add_detects_overflow,

    &op_sub_increments_pc_by_2,
    &op_sub_correctly_subtracts,
    &op_sub_detects_overflow,

    &op_mul_increments_pc_by_2,
    &op_mul_correctly_multiplies,
    &op_mul_detects_overflow,

    &op_div_increments_pc_by_2,
    &op_div_correctly_divides,
    &op_div_detects_overflow,
    &op_div_detects_div_by_zero,
    NULL
};


TEST_RUNNER("Fixnum-Operations", tests, constructor, NULL, setup, teardown)
