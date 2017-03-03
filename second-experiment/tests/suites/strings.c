#define AT_SUITE_NAME Strings
#include <string.h>

#include "../test-suite.h"

#include "objects/strings.h"
#include "type-info.h"


TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry, "org.nuvm.String", &id);

	ASSERT_MSG(type != NULL,
		"Type for strings doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0, "Type for strings has invalid id");
}


TEST(construction_works) {
	const char* str = "Hello, world. Isn't this a lovely day?";
	NError error;
	NString* string = n_string_new(str, &error);

	EXPECT(error.code == N_E_OK);
	ASSERT(string != NULL);
	EXPECT(n_is_string(n_wrap_pointer(string)));
	EXPECT(n_string_length(string) == strlen(str));
	EXPECT(strcmp(n_string_contents(string), str) == 0);
}


TEST(construction_with_null_contents_fails) {
	NError error;
	n_string_new(NULL, &error);
	ASSERT_MSG(error.code != N_E_OK,
		"Construction of string of null contents failed to report "
		"any error at all.");
	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Construction of string of null contents reported wrong error "
		"code. Expected %u, got %u.",
		N_E_INVALID_ARGUMENT, error.code);
	EXPECT_MSG(strcmp(error.message, "contents") == 0,
		"Construction of string of null contents reported wrong error "
		"message. Expected \"%s\", got \"%s\".",
		"contents", error.message);
}


TEST(strings_of_different_contents_are_not_equal) {
	NString* s1 = n_string_new("Hello", NULL);
	NString* s2 = n_string_new("World", NULL);

	ASSERT(!n_string_is_equal(s1, s2));
}

TEST(strings_of_equal_contents_are_equal) {
	char str1[6];
	char str2[20];
	NString* s1;
	NString* s2;

	strcpy(str1, "Hello");
	strcpy(str2, "Hello");

	/* Assertion below is obviously true, but just to make clear
	 * what we intend to test here. */
	ASSERT(str1 != str2);

	s1 = n_string_new(str1, NULL);
	s2 = n_string_new(str2, NULL);

	/* Again, just making sure there's no implementation optimization
	 * that could make those the same string while we try to test for
	 * equality. */
	ASSERT(s1 != s2);

	ASSERT(n_string_is_equal(s1, s2));
}
