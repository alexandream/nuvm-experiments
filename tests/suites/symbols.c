#define AT_SUITE_NAME Symbols
#include <string.h>

#include "../test-suite.h"

#include "objects/symbols.h"
#include "type-info.h"


TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry, "org.nuvm.Symbol", &id);

	ASSERT_MSG(type != NULL,
		"Type for symbols doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0, "Type for strings has invalid id");
}
