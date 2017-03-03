#include <stdlib.h>
#include "common.h"

#ifndef STACK_TYPE_NAME
#error "STACK_TYPE_NAME must be defined to use the generic stack template."
#endif

#ifndef STACK_CONTENTS_TYPE
#error "STACK_CONTENTS_TYPE must be defined to use the generic stack template."
#endif

#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR malloc
#endif

#ifndef STACK_DEALLOCATOR
#define STACK_DEALLOCATOR free
#endif

#ifndef STACK_COPY_ELEMENT
#define STACK_COPY_ELEMENT(E1, E2) *E1 = *E2
#endif

#define STACK_INIT PREFIX_NAME(STACK_TYPE_NAME, init)
#define STACK_PUSH  PREFIX_NAME(STACK_TYPE_NAME, push)
#define STACK_POP  PREFIX_NAME(STACK_TYPE_NAME, pop)
#define STACK_DESTROY PREFIX_NAME(STACK_TYPE_NAME, destroy)
#define STACK_SIZE PREFIX_NAME(STACK_TYPE_NAME, size)
#define STACK_ELEMENTS PREFIX_NAME(STACK_TYPE_NAME, elements)


typedef struct {
	uint32_t size;
	uint32_t capacity;
	STACK_CONTENTS_TYPE* elements;
} STACK_TYPE_NAME;


static inline
void STACK_INIT(STACK_TYPE_NAME*stack, uint32_t initial_capacity) {
	size_t elem_size = sizeof(STACK_CONTENTS_TYPE);
	stack->size = 0;
	stack->capacity = initial_capacity;
	stack->elements =
		(STACK_CONTENTS_TYPE*) STACK_ALLOCATOR(elem_size*initial_capacity);
}

static inline
STACK_CONTENTS_TYPE STACK_POP(STACK_TYPE_NAME*stack) {
	assert(stack->size > 0);
	return stack->elements[--(stack->size)];
}

static inline
uint32_t STACK_PUSH(STACK_TYPE_NAME* stack, STACK_CONTENTS_TYPE value) {
	if (stack->size >= stack->capacity) {
		// Resize the stack so it can keep up with the next element.
		size_t elem_size = sizeof(STACK_CONTENTS_TYPE);
		uint32_t new_capacity = stack->capacity * 2;

		assert(new_capacity > stack->size);

		STACK_CONTENTS_TYPE* new_pool =
			(STACK_CONTENTS_TYPE*) STACK_ALLOCATOR(elem_size*new_capacity);

		assert(new_pool != NULL);

		for(int i = 0; i < stack->capacity; i++) {
			do {
				STACK_COPY_ELEMENT(&new_pool[i], &stack->elements[i]);
			} while(0);
		}
		STACK_DEALLOCATOR(stack->elements);
		stack->elements = new_pool;
	}
	uint32_t index = stack->size++;
	do {
		STACK_COPY_ELEMENT(&stack->elements[index], &value);
	} while(0);
	return index;
}

static inline
void STACK_DESTROY(STACK_TYPE_NAME* stack) {
	STACK_DEALLOCATOR(stack->elements);
}

static inline
uint32_t STACK_SIZE(STACK_TYPE_NAME* stack) {
	return stack->size;
}

static inline
STACK_CONTENTS_TYPE* STACK_ELEMENTS(STACK_TYPE_NAME* stack) {
	return stack->elements;
}

#undef STACK_TYPE_NAME
#undef STACK_CONTENTS_TYPE
#undef STACK_COPY_ELEMENT
#undef STACK_ALLOCATOR
#undef STACK_DEALLOCATOR
#undef STACK_INIT
#undef STACK_PUSH
#undef STACK_POP
#undef STACK_DESTROY
#undef STACK_SIZE
