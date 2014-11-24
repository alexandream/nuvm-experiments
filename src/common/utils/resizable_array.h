#include <assert.h>
#include <stdlib.h>

#include "../polyfills/p-stdint.h"
#include "prefix-macro.h"

#ifndef ARRAY_TYPE_NAME
#error "ARRAY_TYPE_NAME must be defined to use the generic resizable array template."
#endif

#ifndef ARRAY_CONTENTS_TYPE
#error "ARRAY_CONTENTS_TYPE must be defined to use the generic resizable array template."
#endif


#ifndef ARRAY_ALLOCATOR
#define ARRAY_ALLOCATOR malloc
#endif

#ifndef ARRAY_COPY_ELEMENT
#define ARRAY_COPY_ELEMENT(E1, E2) *E1 = *E2
#endif

#ifndef ARRAY_DEALLOCATOR
#define ARRAY_DEALLOCATOR free
#endif

#ifndef ARRAY_PREFIX
#define ARRAY_PREFIX ARRAY_TYPE_NAME
#endif

#ifndef ARRAY_REALLOCATOR
#define ARRAY_REALLOCATOR realloc
#endif

#define ARRAY_INIT     NI_PREFIX_NAME(ARRAY_PREFIX, init)
#define ARRAY_DESTROY  NI_PREFIX_NAME(ARRAY_PREFIX, destroy)

#define ARRAY_APPEND   NI_PREFIX_NAME(ARRAY_PREFIX, append)
#define ARRAY_CAPACITY NI_PREFIX_NAME(ARRAY_PREFIX, capacity)
#define ARRAY_COUNT    NI_PREFIX_NAME(ARRAY_PREFIX, count)
#define ARRAY_ELEMENTS NI_PREFIX_NAME(ARRAY_PREFIX, elements)
#define ARRAY_GET      NI_PREFIX_NAME(ARRAY_PREFIX, get)
#define ARRAY_RESIZE   NI_PREFIX_NAME(ARRAY_PREFIX, resize)
#define ARRAY_SET      NI_PREFIX_NAME(ARRAY_PREFIX, set)

typedef struct {
	int32_t count;
	int32_t capacity;
	ARRAY_CONTENTS_TYPE* elements;
} ARRAY_TYPE_NAME;

static
int32_t ARRAY_CAPACITY(ARRAY_TYPE_NAME* array);

static
int32_t ARRAY_RESIZE(ARRAY_TYPE_NAME* array, int32_t size);

static
void ARRAY_SET(ARRAY_TYPE_NAME*, int32_t, ARRAY_CONTENTS_TYPE);



static
void ARRAY_INIT(ARRAY_TYPE_NAME*array, int32_t size) {
	size_t mem_size = sizeof(ARRAY_CONTENTS_TYPE) * size;
	array->capacity = size;
	array->count = 0;
	array->elements = ARRAY_ALLOCATOR(mem_size);
}


#ifndef ARRAY_DESTROY__SKIP
static
void ARRAY_DESTROY(ARRAY_TYPE_NAME* array) {
	ARRAY_DEALLOCATOR(array->elements);
}
#endif

#ifndef ARRAY_APPEND__SKIP
static
int32_t ARRAY_APPEND(ARRAY_TYPE_NAME* array, ARRAY_CONTENTS_TYPE value) {
	int32_t capacity = ARRAY_CAPACITY(array);
	int32_t index;
	if (capacity == array->count) {
		capacity = (capacity != 0) ? 2 * capacity : 64;
		if(ARRAY_RESIZE(array, capacity) == -1) {
			return -1;
		}
	}
	ARRAY_SET(array, array->count, value);
	index = array->count;
	array->count++;
	return index;
}
#endif


static
int32_t ARRAY_CAPACITY(ARRAY_TYPE_NAME* array) {
	return array->capacity;
}


static
int32_t ARRAY_COUNT(ARRAY_TYPE_NAME* array) {
	return array->count;
}


#ifndef ARRAY_ELEMENTS__SKIP
static
ARRAY_CONTENTS_TYPE* ARRAY_ELEMENTS(ARRAY_TYPE_NAME* array) {
	return array->elements;
}
#endif


static
ARRAY_CONTENTS_TYPE ARRAY_GET(ARRAY_TYPE_NAME* array, int32_t index) {
	ARRAY_CONTENTS_TYPE result;
	assert(index >= 0);
	do {
		ARRAY_COPY_ELEMENT(&result, &array->elements[index]);
	} while(0);
	return array->elements[index];
}


static
int32_t ARRAY_RESIZE(ARRAY_TYPE_NAME* array, int32_t size) {
	assert(size > 0);
	if (size < array->count) size = array->count;
	array->elements =
		ARRAY_REALLOCATOR(array->elements, size * sizeof(ARRAY_CONTENTS_TYPE));
	if (array->elements == NULL) {
		return -1;
	}

	array->capacity = size;
	return size;
}


static
void ARRAY_SET(ARRAY_TYPE_NAME* array,
               int32_t index,
               ARRAY_CONTENTS_TYPE value) {
	assert(index >= 0);
	do {
		ARRAY_COPY_ELEMENT(&array->elements[index], &value);
	} while(0);
}


#undef ARRAY_TYPE_NAME
#undef ARRAY_CONTENTS_TYPE

#undef ARRAY_ALLOCATOR
#undef ARRAY_COPY_ELEMENT
#undef ARRAY_DEALLOCATOR
#undef ARRAY_PREFIX

#undef ARRAY_INIT
#undef ARRAY_DESTROY

#undef ARRAY_APPEND
#undef ARRAY_CAPACITY
#undef ARRAY_COUNT
#undef ARRAY_ELEMENTS
#undef ARRAY_GET
#undef ARRAY_RESIZE
#undef ARRAY_SET

#undef ARRAY_APPEND__SKIP
#undef ARRAY_DESTROY__SKIP
#undef ARRAY_ELEMENTS__SKIP
