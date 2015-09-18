#define SUITE_NAME LabelManager

#include "../../test-suite.h"

#include "label-manager.h"

static NLabelManager* label_man = NULL;

SETUP {
	label_man = NULL;
}


TEARDOWN {
	if (label_man != NULL) {
		ni_destroy_label_manager(label_man);
	}
}


TEST(label_manager_creation_works) {
	NError error = N_ERROR_INITIALIZER;
	
	label_man = ni_new_label_manager(&error);

	ASSERT(ERROR_OK(&error));
	ASSERT(IS_TRUE(label_man != NULL));
}

TEST(first_label_comes_with_id_zero) {
	NError error = N_ERROR_INITIALIZER;
	int32_t id;
   
	label_man = ni_new_label_manager(&error);

	id = ni_label_manager_get(label_man, "hello", NULL);
	ASSERT(EQ_INT(id, 0));
}


TEST(repeated_first_label_comes_with_id_zero) {
	NError error = N_ERROR_INITIALIZER;
	int32_t id;
   
	label_man = ni_new_label_manager(&error);
	
	ni_label_manager_get(label_man, "hello", NULL);
	id = ni_label_manager_get(label_man, "hello", NULL);

	ASSERT(EQ_INT(id, 0));
}


TEST(repeated_labels_ignore_case) {
	NError error = N_ERROR_INITIALIZER;
	int32_t id;
   
	label_man = ni_new_label_manager(&error);

	ni_label_manager_get(label_man, "hello", NULL);
	id = ni_label_manager_get(label_man, "hElLo", NULL);

	ASSERT(EQ_INT(id, 0));
}


TEST(new_label_increases_id) {
	NError error = N_ERROR_INITIALIZER;
	int32_t id;
   
	label_man = ni_new_label_manager(&error);

	ni_label_manager_get(label_man, "first_label", NULL);
	id = ni_label_manager_get(label_man, "second_label", NULL);

	ASSERT(EQ_INT(id, 1));
}


TEST(label_definition_to_zero_works) {
	NError error = N_ERROR_INITIALIZER;
	uint32_t definition;
	
	label_man = ni_new_label_manager(NULL);

	ni_label_manager_define(label_man, "my_label", 0, &error);
	ASSERT(ERROR_OK(&error));
	
	definition =
		ni_label_manager_get_definition(label_man, "my_label", &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(definition, 0));
}


TEST(label_definition_to_one_works) {
	NError error = N_ERROR_INITIALIZER;
	uint32_t definition;
	
	label_man = ni_new_label_manager(NULL);

	ni_label_manager_define(label_man, "my_label", 1, &error);
	ASSERT(ERROR_OK(&error));
	
	definition =
		ni_label_manager_get_definition(label_man, "my_label", &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(definition, 1));
}
