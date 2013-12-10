#define AT_SUITE_NAME Module
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "module.h"
#include "value.h"

TEST(empty_module_construction_fails) {
	NError error;
	
	NModule* mod = n_new_module(0, 0, &error);

	ASSERT_MSG(error.code != N_E_OK,
	           "Construction of empty module failed to report "
	           "any error at all");

	ASSERT_MSG(error.code == N_E_INVALID_ARGUMENT,
	           "Construction of empty module failed to report "
	           "invalid argument error.");

	ASSERT_MSG(strcmp(error.message, "register_count") == 0,
	           "Construction of empty module reported invalid argument error "
	           "under wrong argument name [%s], expected \"register_count\"",
	           error.message);
	n_destroy_module(mod);
}
