#define AT_SUITE_NAME Value_Operations
#include "../test-suite.h"

#include "value.h"
#include "type-info.h"

typedef struct {
	const char* name;
	union {
		NValue value;
		void*  pointer;
		int32_t fixnum;
	} item;
} NamedItem;

typedef struct {
	NamedItem** items;
	int32_t capacity;
	int32_t count;
} ItemList;

/* Auxiliary functions declaration. */

static int32_t _add_item(ItemList* self, NamedItem* item);

static void _destroy_list(ItemList* list);

static ItemList* _get_fixnums();

static ItemList* _get_pointers();

static ItemList* _get_singletons();

static ItemList* _get_values();

static ItemList* _new_item_list();



/* Test functions. */

TEST(basic_types_have_been_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();

	EXPECT(n_type_registry_has_type(registry, "org.nuvm.Boolean"));
	EXPECT(n_type_registry_has_type(registry, "org.nuvm.Fixnum32"));
	EXPECT(n_type_registry_has_type(registry, "org.nuvm.Undefined"));
}


TEST(booleans_have_boolean_type) {
	EXPECT(n_is_boolean(N_TRUE));
	EXPECT(n_is_boolean(N_FALSE));
}


TEST(different_values_are_not_equals) {
	int32_t i, j;
	ItemList* values = _get_values();

	for (i = 0; i < values->count; i++) {
		for(j = 0; j < values->count; j++) {
			if (j != i) {
				NamedItem* vi = values->items[i];
				NamedItem* vj = values->items[j];
				EXPECT_MSG(!n_is_equal(vi->item.value, vj->item.value),
			               "Value [%s] reported equals to [%s].",
				           vi->name, vj->name);
			}
		}
	}
	_destroy_list(values);
}


TEST(singletons_are_pointers) {
	ItemList* list = _get_singletons();
	int32_t i;

	for (i = 0; i < list->count; i ++) {
		const char* name = list->items[i]->name;
		NValue singleton = list->items[i]->item.value;

		EXPECT_MSG(n_is_pointer(singleton),
			"Singleton %s failed to be recognized as pointer.",
			name);
	}
	_destroy_list(list);
}


TEST(undefined_has_undefined_type) {
	EXPECT(n_is_undefined(N_UNDEFINED));
}


TEST(values_are_equals_to_themselves) {
	int32_t i;
	ItemList* values = _get_values();

	for (i = 0; i < values->count; i++) {
		NamedItem* item = values->items[i];
		EXPECT_MSG(n_is_equal(item->item.value, item->item.value),
		           "Value [%s] failed to report equals to itself.",
		           item->name);
	}
	_destroy_list(values);
}


TEST(wrapped_fixnums_are_fixnums) {
	ItemList* list = _get_fixnums();
	int32_t i;
	for (i = 0; i < list->count; i++) {
		const char* name = list->items[i]->name;
		int32_t fixnum = list->items[i]->item.fixnum;
		NValue value = n_wrap_fixnum(fixnum);
		EXPECT_MSG(n_is_fixnum(value),
			"Fixnum %s [%d] failed to be recognized as fixnum "
			"after wrapping.",
			name, fixnum);
	}
}


TEST(wrapped_fixnums_are_not_pointers) {
	ItemList* list = _get_fixnums();
	int32_t i;
	for (i = 0; i < list->count; i++) {
		const char* name = list->items[i]->name;
		int32_t fixnum = list->items[i]->item.fixnum;
		NValue value = n_wrap_fixnum(fixnum);
		EXPECT_MSG(!n_is_pointer(value),
			"Fixnum %s [%d] was recognized as pointer after wrapping.",
			name, fixnum);
	}
}


TEST(wrapped_pointers_are_pointers) {
	ItemList* list = _get_pointers();
	int32_t i;
	for (i = 0; i < list->count; i++) {
		const char* name = list->items[i]->name;
		void* pointer = list->items[i]->item.pointer;
		NValue value = n_wrap_pointer(pointer);
		EXPECT_MSG(n_is_pointer(value),
			"Pointer %s [%p] failed to be recognized "
			"as pointer after wrapping.",
			name, pointer);
	}
	_destroy_list(list);
}

TEST(wrapped_pointers_are_not_fixnums) {
	ItemList* list = _get_pointers();
	int32_t i;
	for (i = 0; i < list->count; i++) {
		const char* name = list->items[i]->name;
		void* pointer = list->items[i]->item.pointer;
		NValue value = n_wrap_pointer(pointer);
		EXPECT_MSG(!n_is_fixnum(value),
			"Pointer %s [%p] was recognized as fixnum after wrapping.",
			name, pointer);
	}
	_destroy_list(list);
}


TEST(wrapped_pointers_unwrap_to_original_value) {
	ItemList* list = _get_pointers();
	int32_t i;
	for (i = 0; i < list->count; i++) {
		const char* name = list->items[i]->name;
		void* pointer = list->items[i]->item.pointer;
		NValue value = n_wrap_pointer(pointer);
		void* out_pointer = n_unwrap_pointer(value);

		EXPECT_MSG(pointer == out_pointer,
		           "After wrapping and unwrapping, pointer %s [%p] "
		           "reported final value %p.",
		           name, pointer, out_pointer);
	}
	_destroy_list(list);
}


/* Internal utility functions. */

static NamedItem*
_new_named_item(const char* name) {
	NamedItem* result = malloc(sizeof(NamedItem));
	assert(result != NULL);

	result->name = name;
	return result;
}


static NamedItem*
_np(const char* name, void* pointer) {
	NamedItem* result = _new_named_item(name);
	result->item.pointer = pointer;
	return result;
}


static NamedItem*
_nfix(const char* name, int32_t fixnum) {
	NamedItem* result = _new_named_item(name);
	result->item.fixnum = fixnum;
	return result;
}

static NamedItem*
_nv(const char* name, NValue value) {
	NamedItem* result = _new_named_item(name);
	result->item.value = value;
	return result;
}


static void
_resize_list(ItemList* list) {
	int32_t new_capacity = list->capacity ? 2 * list->capacity
										  : 8;
	list->items = realloc(list->items, new_capacity * sizeof(NamedItem));
	assert(list->items != NULL);
	list->capacity = new_capacity;
}

/* Utility functions. */


static int32_t
_add_item(ItemList* list, NamedItem* item) {
	int32_t index = list->count;
	if (index == list->capacity) {
		_resize_list(list);
	}
	list->items[index] = item;
	list->count = index + 1;
	return index;
}


static void
_destroy_list(ItemList* list) {
	int32_t i;
	for (i = 0; i < list->count; i++) {
		free(list->items[i]);
	}
	free(list);
}


static ItemList*
_get_fixnums() {
	ItemList* list = _new_item_list();
	_add_item(list, _nfix("Minimum Fixnum", -2147483648));
	_add_item(list, _nfix("Maximum Fixnum", 2147483647));
	_add_item(list, _nfix("Zero", 0));
	_add_item(list, _nfix("Small Positive Fixnum", 1));
	_add_item(list, _nfix("Small Negative Fixnum", -1));
	return list;
}


static ItemList*
_get_pointers() {
	ItemList* list = _new_item_list();
	void* heap_addr = malloc(sizeof(NValue));
	_add_item(list, _np("Null Pointer", NULL));
	_add_item(list, _np("Unmanaged Heap Address", heap_addr));

	free(heap_addr);
	return list;
}


static ItemList*
_get_singletons() {
	ItemList* list = _new_item_list();
	_add_item(list, _nv("Boolean True", N_TRUE));
	_add_item(list, _nv("Boolean False", N_FALSE));
	_add_item(list, _nv("Undefined", N_UNDEFINED));
	return list;
}


static ItemList*
_get_values() {
	ItemList* list = _new_item_list();
	ItemList* pointer_list, *fixnum_list;
	int32_t i;

	pointer_list = _get_pointers();
	for (i = 0; i < pointer_list->count; i++) {
		NamedItem* ni = pointer_list->items[i];
		_add_item(list, _nv(ni->name, n_wrap_pointer(ni->item.pointer)));
	}
	_destroy_list(pointer_list);

	fixnum_list = _get_fixnums();
	for (i = 0; i < fixnum_list->count; i++) {
		NamedItem* ni = fixnum_list->items[i];
		_add_item(list, _nv(ni->name, n_wrap_fixnum(ni->item.fixnum)));
	}
	_destroy_list(fixnum_list);

	return list;
}

static ItemList*
_new_item_list() {
	ItemList* list = malloc(sizeof(ItemList));
	assert(list != NULL);

	list->items = NULL;
	list->capacity = 0;
	list->count = 0;
	return list;
}
