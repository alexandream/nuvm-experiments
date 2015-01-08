#include "preamble.h"

#define ARRAY__STRUCT__SKIP
#include "header.h"
#undef ARRAY__STRUCT__SKIP


ARRAY_STATIC
void ARRAY_INIT(ARRAY_TYPE_NAME*array, int32_t size) {
	size_t mem_size = sizeof(ARRAY_CONTENTS_TYPE) * size;
	array->capacity = size;
	array->count = 0;
	array->elements = ARRAY_ALLOCATOR(mem_size);
}


#ifndef ARRAY_DESTROY__SKIP
ARRAY_STATIC
void ARRAY_DESTROY(ARRAY_TYPE_NAME* array) {
	ARRAY_DEALLOCATOR(array->elements);
}
#endif

#ifndef ARRAY_APPEND__SKIP
ARRAY_STATIC
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


ARRAY_STATIC
int32_t ARRAY_CAPACITY(ARRAY_TYPE_NAME* array) {
	return array->capacity;
}


ARRAY_STATIC
int32_t ARRAY_COUNT(ARRAY_TYPE_NAME* array) {
	return array->count;
}


#ifndef ARRAY_ELEMENTS__SKIP
ARRAY_STATIC
ARRAY_CONTENTS_TYPE* ARRAY_ELEMENTS(ARRAY_TYPE_NAME* array) {
	return array->elements;
}
#endif


ARRAY_STATIC
ARRAY_CONTENTS_TYPE ARRAY_GET(ARRAY_TYPE_NAME* array, int32_t index) {
	ARRAY_CONTENTS_TYPE result;
	assert(index >= 0);
	do {
		ARRAY_COPY_ELEMENT(&result, &array->elements[index]);
	} while(0);
	return array->elements[index];
}


ARRAY_STATIC
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


ARRAY_STATIC
void ARRAY_SET(ARRAY_TYPE_NAME* array,
               int32_t index,
               ARRAY_CONTENTS_TYPE value) {
	assert(index >= 0);
	do {
		ARRAY_COPY_ELEMENT(&array->elements[index], &value);
	} while(0);
}

#include "undefines.h"
