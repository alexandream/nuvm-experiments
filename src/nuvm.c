#include <stdlib.h>

#include "nuvm.h"

const uint8_t nuvm_version[] = { 0, 0, 1 };

void nuvm_init() {
	/* do nothing so far. */
}

void* nuvm_alloc_unmanaged(size_t size) {
	return malloc(size);
}
