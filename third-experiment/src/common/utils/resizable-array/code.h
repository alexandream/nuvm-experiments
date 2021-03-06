#include "preamble.h"

#define N_DS_ARRAY_P_SKIP_STRUCT
#include "header.h"
#undef N_DS_ARRAY_P_SKIP_STRUCT


N_DS_ARRAY_STATIC
void N_DS_ARRAY_INIT(N_DS_ARRAY_TYPE_NAME*array, int32_t size) {
	size_t mem_size = sizeof(N_DS_ARRAY_CONTENTS_TYPE) * size;
	array->capacity = size;
	array->count = 0;
	array->elements = N_DS_ARRAY_ALLOCATOR(mem_size);
	array->attached = true;
}


#ifndef N_DS_ARRAY_P_SKIP_DESTROY
N_DS_ARRAY_STATIC
void N_DS_ARRAY_DESTROY(N_DS_ARRAY_TYPE_NAME* array) {
	if (array->attached) {
#ifdef N_DS_ARRAY_DESTROY_ELEMENT
		int i;
		for (i = 0; i < array->count; i++) {
			N_DS_ARRAY_DESTROY_ELEMENT(array->elements[i]);
		}
#endif
		N_DS_ARRAY_DEALLOCATOR(array->elements);
	}
}
#endif

#ifndef N_DS_ARRAY_P_SKIP_DETACH
N_DS_ARRAY_STATIC
void N_DS_ARRAY_DETACH(N_DS_ARRAY_TYPE_NAME* array) {
	array->attached = false;
	array->capacity = 0;
	array->count = 0;
	array->elements = NULL;
}
#endif

#ifndef N_DS_ARRAY_P_SKIP_APPEND
N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_APPEND(N_DS_ARRAY_TYPE_NAME* array, N_DS_ARRAY_CONTENTS_TYPE value) {
	int32_t capacity = N_DS_ARRAY_CAPACITY(array);
	int32_t index;
	if (array->attached) {
		if (capacity == array->count) {
			capacity = (capacity != 0) ? 2 * capacity : 64;
			if(N_DS_ARRAY_RESIZE(array, capacity) == -1) {
				return -1;
			}
		}
		N_DS_ARRAY_SET(array, array->count, value);
		index = array->count;
		array->count++;
		return index;
	}
	else {
		return -2;
	}
}
#endif


N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_CAPACITY(N_DS_ARRAY_TYPE_NAME* array) {
	return array->capacity;
}


N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_COUNT(N_DS_ARRAY_TYPE_NAME* array) {
	return array->count;
}


#ifndef N_DS_ARRAY_P_SKIP_ELEMENTS
N_DS_ARRAY_STATIC
N_DS_ARRAY_CONTENTS_TYPE* N_DS_ARRAY_ELEMENTS(N_DS_ARRAY_TYPE_NAME* array) {
	return array->elements;
}
#endif


#ifndef N_DS_ARRAY_P_SKIP_GET
N_DS_ARRAY_STATIC
N_DS_ARRAY_CONTENTS_TYPE N_DS_ARRAY_GET(N_DS_ARRAY_TYPE_NAME* array, int32_t index) {
	return *N_DS_ARRAY_GET_REF(array, index);
}
#endif


N_DS_ARRAY_STATIC
N_DS_ARRAY_CONTENTS_TYPE* N_DS_ARRAY_GET_REF(N_DS_ARRAY_TYPE_NAME* array, int32_t index) {
	assert(index >= 0);
	return &(array->elements[index]);
}

N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_RESIZE(N_DS_ARRAY_TYPE_NAME* array, int32_t size) {
	if (array->attached) {
		assert(size > 0);
		if (size < array->count) size = array->count;
		array->elements =
			N_DS_ARRAY_REALLOCATOR(array->elements, size * sizeof(N_DS_ARRAY_CONTENTS_TYPE));
		if (array->elements == NULL) {
			return -1;
		}

		array->capacity = size;
		return size;
	}
	else {
		return 0;
	}
}


N_DS_ARRAY_STATIC
void N_DS_ARRAY_SET(N_DS_ARRAY_TYPE_NAME* array,
               int32_t index,
               N_DS_ARRAY_CONTENTS_TYPE value) {
	assert(index >= 0);
	do {
		N_DS_ARRAY_COPY_ELEMENT(&array->elements[index], &value);
	} while(0);
}

#include "undefines.h"
