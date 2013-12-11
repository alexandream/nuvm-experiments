#include <stdlib.h>

#include "memory.h"

void* n_alloc_unmanaged(size_t size) {
	return malloc(size);
}

void n_free(void* mem) {
	free(mem);
}
