#define TEST_CASE_NAME "Procedure Type"
#include "../test-case.h"

#include "nuvm.h"
#include "module.h"
#include "types/procedure.h"

BEGIN_TEST(test_procedure_structure_sizes) {
	fail_if(nuvm_procedure_t_size() < sizeof(nuvm_object_t));
} END_TEST


BEGIN_TEST(test_procedure_construction) {
	nuvm_module_t* mod;
	nuvm_procedure_t* proc = nuvm_new_procedure(mod, 16, 3);
	fail_if(mod != nuvm_procedure_get_module(proc));
	fail_if(16   != nuvm_procedure_get_entry_point(proc));
	fail_if(3    != nuvm_procedure_get_num_locals(proc));
} END_TEST



