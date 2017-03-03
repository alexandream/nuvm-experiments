#include <stdlib.h>
#include "../test.h"

#include "common/errors.h"
#include "common/instructions.h"
#include "eval/evaluator.h"


#define CODE_SIZE 16
#define NUM_REGISTERS 16

typedef struct {
    int left;
    int right;
    int expected;
} BinopData;


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


TEST(op_load_bool_increments_pc_by_1) {
    n_encode_op_load_bool(CODE, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 1));
}


TEST(op_load_bool_0_returns_false) {
    uint8_t dest = 3;
    NValue result;

    n_encode_op_load_bool(CODE, dest, 0);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);
    ASSERT(IS_OK(ERROR));
    ASSERT(IS_TRUE(n_eq_values(result, N_FALSE)));
}


TEST(op_load_bool_1_returns_true) {
    uint8_t dest = 3;
    NValue result;

    n_encode_op_load_bool(CODE, dest, 1);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);
    ASSERT(IS_OK(ERROR));
    ASSERT(IS_TRUE(n_eq_values(result, N_TRUE)));
}


TEST(op_not_increments_pc_by_1) {
    n_encode_op_not(CODE, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 1));
}


TEST(op_not_true_returns_false) {
    uint8_t dest = 3;
    uint8_t arg = 7;
    NValue result;

    REGISTERS[arg] = N_TRUE;
    n_encode_op_not(CODE, dest, arg);

    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);
    ASSERT(IS_OK(ERROR));
    ASSERT(IS_TRUE(n_eq_values(result, N_FALSE)));
}


TEST(op_not_false_returns_true) {
    uint8_t dest = 3;
    uint8_t arg = 7;
    NValue result;

    REGISTERS[arg] = N_FALSE;
    n_encode_op_not(CODE, dest, arg);

    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);
    ASSERT(IS_OK(ERROR));
    ASSERT(IS_TRUE(n_eq_values(result, N_TRUE)));

}


TEST(op_and_increments_pc_by_2) {
    n_encode_op_and(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


BinopData and_table_a[] = {
    { 0, 0, 0 },
    { 0, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 1 }
};
AtArrayIterator and_table_i = at_static_array_iterator(and_table_a);
DD_TEST(op_and_respects_truth_table, and_table_i, BinopData, entry) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NValue left  = n_wrap_boolean(entry->left);
    NValue right = n_wrap_boolean(entry->right);
    NValue expected = n_wrap_boolean(entry->expected);
    NValue result;

    REGISTERS[arg1] = left;
    REGISTERS[arg2] = right;

    n_encode_op_and(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);

    ASSERT(IS_TRUE(n_eq_values(result, expected)));
}


TEST(op_or_increments_pc_by_2) {
    n_encode_op_or(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


BinopData or_table_a[] = {
    { 0, 0, 0 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 1 }
};
AtArrayIterator or_table_i = at_static_array_iterator(or_table_a);
DD_TEST(op_or_respects_truth_table, or_table_i, BinopData, entry) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NValue left  = n_wrap_boolean(entry->left);
    NValue right = n_wrap_boolean(entry->right);
    NValue expected = n_wrap_boolean(entry->expected);
    NValue result;

    REGISTERS[arg1] = left;
    REGISTERS[arg2] = right;

    n_encode_op_or(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);

    ASSERT(IS_TRUE(n_eq_values(result, expected)));
}


TEST(op_xor_increments_pc_by_2) {
    n_encode_op_xor(CODE, 0, 0, 0);
    n_evaluator_step(&EVAL, &ERROR);

    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(EVAL.pc, 2));
}


BinopData xor_table_a[] = {
    { 0, 0, 0 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 0 }
};
AtArrayIterator xor_table_i = at_static_array_iterator(xor_table_a);
DD_TEST(op_xor_respects_truth_table, xor_table_i, BinopData, entry) {
    uint8_t dest = 0;
    uint8_t arg1 = 1;
    uint8_t arg2 = 2;
    NValue left  = n_wrap_boolean(entry->left);
    NValue right = n_wrap_boolean(entry->right);
    NValue expected = n_wrap_boolean(entry->expected);
    NValue result;

    REGISTERS[arg1] = left;
    REGISTERS[arg2] = right;

    n_encode_op_xor(CODE, dest, arg1, arg2);
    n_evaluator_step(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_evaluator_get_register(&EVAL, dest, &ERROR);

    ASSERT(IS_TRUE(n_eq_values(result, expected)));
}

AtTest* tests[] = {
    &op_load_bool_increments_pc_by_1,
    &op_load_bool_0_returns_false,
    &op_load_bool_1_returns_true,

    &op_not_increments_pc_by_1,
    &op_not_true_returns_false,
    &op_not_false_returns_true,
    
    &op_and_increments_pc_by_2,
    &op_and_respects_truth_table,

    &op_or_increments_pc_by_2,
    &op_or_respects_truth_table,

    &op_xor_increments_pc_by_2,
    &op_xor_respects_truth_table,
    NULL
};


TEST_RUNNER("Boolean-Operations", tests, constructor, NULL, setup, teardown)
