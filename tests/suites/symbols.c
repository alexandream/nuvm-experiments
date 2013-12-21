#define AT_SUITE_NAME Symbols
#include <string.h>

#include "../test-suite.h"

#include "symbol-pool.h"
#include "type-info.h"
#include "value.h"


TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry, "org.nuvm.Symbol", &id);

	ASSERT_MSG(type != NULL,
		"Type for symbols doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0, "Type for symbols has invalid id");
}


TEST(construction_works) {
	const char* str = "this-is-a-somewhat-long-symbol-but-not-too-much";
	NValue symbol = n_symbol_pool_get_symbol(str);

	ASSERT(n_is_symbol(symbol));
	EXPECT(strcmp(n_symbol_pool_fetch(n_unwrap_symbol(symbol)), str) == 0);
}


TEST(construction_with_null_contents_returns_undefined) {
	NValue symbol = n_symbol_pool_get_symbol(NULL);
	ASSERT(n_is_equal(symbol, N_UNDEFINED));
}


TEST(two_different_symbols_are_not_equal) {
	NValue symbol1 = n_symbol_pool_get_symbol("Hello");
	NValue symbol2 = n_symbol_pool_get_symbol("World");

	ASSERT(!n_is_equal(symbol1, symbol2));
}


TEST(symbols_of_same_contents_are_equal) {
	NValue symbol1 = n_symbol_pool_get_symbol("Hello");
	NValue symbol2 = n_symbol_pool_get_symbol("Hello");

	ASSERT(n_is_equal(symbol1, symbol2));
}

