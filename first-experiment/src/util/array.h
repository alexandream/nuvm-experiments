#include <stdlib.h>
#include "common.h"

#ifndef ARRAY_TYPE_NAME
#error "ARRAY_TYPE_NAME must be defined to use the generic array template."
#endif

#ifndef ARRAY_CONTENTS_TYPE
#error "ARRAY_CONTENTS_TYPE must be defined to use the generic array template."
#endif

#ifndef ARRAY_ALLOCATOR
#define ARRAY_ALLOCATOR malloc
#endif

#ifndef ARRAY_DEALLOCATOR
#define ARRAY_DEALLOCATOR free
#endif

#ifndef ARRAY_COPY_ELEMENT
#define ARRAY_COPY_ELEMENT(E1, E2) *E1 = *E2
#endif

#define ARRAY_INIT PREFIX_NAME(ARRAY_TYPE_NAME, init)
#define ARRAY_GET  PREFIX_NAME(ARRAY_TYPE_NAME, get)
#define ARRAY_SET  PREFIX_NAME(ARRAY_TYPE_NAME, set)
#define ARRAY_DESTROY PREFIX_NAME(ARRAY_TYPE_NAME, destroy)
#define ARRAY_SIZE PREFIX_NAME(ARRAY_TYPE_NAME, size)
#define ARRAY_ELEMENTS PREFIX_NAME(ARRAY_TYPE_NAME, elements)

typedef struct {
	uint32_t size;
	ARRAY_CONTENTS_TYPE* elements;
} ARRAY_TYPE_NAME;


static inline
void ARRAY_INIT(ARRAY_TYPE_NAME*array, uint32_t size) {
	array->size = size;
	array->elements =
		(ARRAY_CONTENTS_TYPE*) malloc(sizeof(ARRAY_CONTENTS_TYPE)*size);
}

static inline
ARRAY_CONTENTS_TYPE ARRAY_GET(ARRAY_TYPE_NAME*array, uint32_t index) {
	return array->elements[index];
}

static inline
void ARRAY_SET(ARRAY_TYPE_NAME* array,
               uint32_t index,
               ARRAY_CONTENTS_TYPE value) {
	do {
		ARRAY_COPY_ELEMENT(&array->elements[index], &value);
	} while(0);
}

static inline
void ARRAY_DESTROY(ARRAY_TYPE_NAME* array) {
	free(array->elements);
}

static inline
uint32_t ARRAY_SIZE(ARRAY_TYPE_NAME* array) {
	return array->size;
}

static inline
ARRAY_CONTENTS_TYPE* ARRAY_ELEMENTS(ARRAY_TYPE_NAME* array) {
	return array->elements;
}

#undef ARRAY_TYPE_NAME
#undef ARRAY_CONTENTS_TYPE
#undef ARRAY_COPY_ELEMENT
#undef ARRAY_ALLOCATOR
#undef ARRAY_DEALLOCATOR
#undef ARRAY_INIT
#undef ARRAY_GET
#undef ARRAY_SET
#undef ARRAY_DESTROY
#undef ARRAY_SIZE
#undef ARRAY_ELEMENTS
