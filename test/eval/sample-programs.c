#include <stdlib.h>
#include <stdio.h>
#include "../test.h"
#include "common/errors.h"

#include "common/instructions.h"
#include "eval/evaluator.h"
#include "eval/fixnum.h"


#define CODE_SIZE 32
#define NUM_REGISTERS 16

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
        n_encode_op_nop(CODE+i);
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


TEST(sum_of_arithmetic_progression) {
    int offset = 0;
    NFixnum result;
    /* The code below computes the sum of the arithmetic sequence with
     * initial value (register 0) = 1, common difference (register 1) = 3
     * for the first 20 elements (register 2). The expected result is 590. */
    offset += n_encode_op_load_int4(CODE, 3, 2);
    offset += n_encode_op_div(CODE+offset, 3, 2, 3);
    offset += n_encode_op_load_int4(CODE+offset, 4, 1);
    offset += n_encode_op_sub(CODE+offset, 4, 2, 4);
    offset += n_encode_op_mul(CODE+offset, 4, 4, 1);
    offset += n_encode_op_load_int4(CODE+offset, 5, 2);
    offset += n_encode_op_mul(CODE+offset, 5, 0, 5);
    offset += n_encode_op_add(CODE+offset, 4, 4, 5);
    offset += n_encode_op_mul(CODE+offset, 3, 3, 4);
    offset += n_encode_op_halt(CODE+offset);
    
    REGISTERS[0] = n_wrap_fixnum(1);
    REGISTERS[1] = n_wrap_fixnum(3);
    REGISTERS[2] = n_wrap_fixnum(20);

    n_evaluator_run(&EVAL, &ERROR);
    ASSERT(IS_OK(ERROR));

    result = n_unwrap_fixnum(n_evaluator_get_register(&EVAL, 3, &ERROR));
    ASSERT(IS_OK(ERROR));
    ASSERT(EQ_INT(result, 590));
}


AtTest* tests[] = {
    &sum_of_arithmetic_progression,
    NULL
};


TEST_RUNNER("Sample-Programs", tests, constructor, NULL, setup, teardown)
