#define AT_SUITE_NAME Evaluator
#include "../test-suite.h"

#include <string.h>

#include "evaluator.h"
#include "error.h"
#include "value.h"

TEST(running_uninitialized_evaluator_fails) {
	NError error;
	NEvaluator* eval = n_evaluator_new(NULL);

	n_evaluator_run(eval, &error);
	EXPECT_MSG(error.code != N_E_OK,
		"Running uninitialized evaluator failed to report any error at all");

	EXPECT_MSG(error.code == N_E_UNINITIALIZED_EVALUATOR,
		"Running uninitialized evaluator reported wrong error "
		"code: %u. Expected %u.",
		error.code, N_E_UNINITIALIZED_EVALUATOR);

	n_evaluator_destroy(eval);
}

TEST(evaluator_construction_initializes_stack) {
	NEvaluator* eval = n_evaluator_new(NULL);

	EXPECT(n_evaluator_stack_pointer(eval) == 0);
	EXPECT(n_evaluator_stack_capacity(eval) > 0);

	n_evaluator_destroy(eval);
}
