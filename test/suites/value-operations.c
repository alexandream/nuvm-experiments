#define AT_SUITE_NAME Value_Operations
#include "../test-suite.h"

#include "value.h"

typedef struct {
	const char* name;
	union {
		NValue value;
		void*  pointer;
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

static ItemList* _get_pointers();

static ItemList* _get_values();

static ItemList* _new_item_list();



/* Test functions. */

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
_get_pointers() {
	ItemList* list = _new_item_list();
	void* heap_addr = malloc(sizeof(NValue));
	_add_item(list, _np("Null Pointer", NULL));
	_add_item(list, _np("Unmanaged Heap Address", heap_addr));

	free(heap_addr);
	return list;
}


static ItemList*
_get_values() {
	ItemList* list = _new_item_list();
	ItemList* pointer_list;
	int32_t i;
	
	pointer_list = _get_pointers();
	for (i = 0; i < pointer_list->count; i++) {
		NamedItem* ni = pointer_list->items[i];
		_add_item(list, _nv(ni->name, n_wrap_pointer(ni->item.pointer)));
	}
	_destroy_list(pointer_list);
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
