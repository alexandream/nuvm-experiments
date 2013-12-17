#define AT_SUITE_NAME Types
#include "../test-suite.h"

#include <string.h>

#include "error.h"

#include "type-info.h"

TEST(type_construction_sets_name) {
	NType type;
	n_type_init(&type, "foo.bar.Type");

	EXPECT_MSG(strcmp(type.name, "foo.bar.Type") == 0,
		"Expected value of \"%s\" for type.name, got \"%s\".",
		"foo.bar.Type", type.name);
}


TEST(adding_new_type_works) {
	NError error;
	NType type;
	int32_t type_id = -1;
	int32_t type_count;
	NTypeRegistry* registry = n_type_registry_new(NULL);

	n_type_init(&type, "foo.bar.Type");
	type_id = n_type_registry_add_type(registry, &type, &error);

	EXPECT_MSG(error.code == 0,
		"Expected error code of 0, got %u.",
		error.code);

	EXPECT_MSG(type_id >= 0,
		"Type id should be set after adding to a registry, got 0.");

	EXPECT_MSG(strcmp(type.name, "foo.bar.Type") == 0,
		"Type name should be unchanged after adding to a registry. "
		"Expected \"foo.bar.Type\", got %s.",
		type.name);

	type_count = n_type_registry_count_types(registry);
	EXPECT_MSG(type_count == 1,
		"Expected a type count of 1 after registering a type, got %u.",
		type_count);

	n_type_registry_destroy(registry);
}


TEST(adding_repeated_type_name_fails) {
	NError error;
	NType type1, type2;
	NTypeRegistry* registry = n_type_registry_new(NULL);

	n_type_init(&type1, "foo.bar.Type");
	n_type_init(&type2, "foo.bar.Type");

	n_type_registry_add_type(registry, &type1, &error);
	ASSERT(error.code == N_E_OK);

	n_type_registry_add_type(registry, &type2, &error);
	EXPECT_MSG(error.code == N_E_TYPE_REGISTRY_CONFLICT,
		"Adding repeated type name failed to report error. "
		"Expected error code %u, got %u.",
		N_E_TYPE_REGISTRY_CONFLICT, error.code);
	n_type_registry_destroy(registry);
}


TEST(adding_many_types_works) {
	NError error;
	NType type1, type2;
	int32_t id1, id2;
	NTypeRegistry* registry = n_type_registry_new(NULL);

	n_type_init(&type1, "foo.bar.Type1");
	id1 = n_type_registry_add_type(registry, &type1, NULL);

	n_type_init(&type2, "foo.bar.Type2");
	id2 = n_type_registry_add_type(registry, &type2, &error);

	EXPECT_MSG(error.code == N_E_OK,
		"Expected insertion of second type to return success, "
		"got error code %u.",
		error.code);

	EXPECT_MSG(n_type_registry_count_types(registry) == 2,
		"Expected total type count of 2, got %d.",
		n_type_registry_count_types(registry));

	EXPECT_MSG(id1 >= 0,
		"Expected a valid non-negative type id for type 1, got %d.",
		id1);

	EXPECT_MSG(id2 >= 0,
		"Expected a valid non-negative type id for type 2, got %d.",
		id2);

	EXPECT_MSG(id1 != id2,
		"Expected a distinct type id for each type, both got %d.",
		id1);
	n_type_registry_destroy(registry);
}


TEST(find_type_by_name_works) {
	NType type, *out_type;
	int32_t type_id, out_id;
	NTypeRegistry* registry = n_type_registry_new(NULL);

	n_type_init(&type, "foo.bar.Type");
	type_id = n_type_registry_add_type(registry, &type, NULL);

	out_type = n_type_registry_find_type(registry, "foo.bar.Type", &out_id);

	EXPECT_MSG(out_type == &type,
		"Expected type found to have same address as type added [%p]. "
		"Got [%p].",
		&type, out_type);

	EXPECT_MSG(out_id == type_id,
		"Expected type found to have same id as type added [%u]. Got [%u].",
		type_id, out_id);
	n_type_registry_destroy(registry);
}


TEST(find_unregistered_type_returns_null) {
	int32_t index;
	NTypeRegistry* registry = n_type_registry_new(NULL);
	NType* type = n_type_registry_find_type(registry, "foo.bar.Type", &index);

	EXPECT_MSG(type == NULL,
		"Expected search for unregistered type to return NULL, got [%p].",
		type);
	n_type_registry_destroy(registry);
}
