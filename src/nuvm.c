#include <stdlib.h>

#include "nuvm.h"

const uint8_t nuvm_version[] = { 0, 0, 1 };

// FIXME: This should be a real managed allocator for our garbage collection
// system. Turns out we don't have one yet :)
void* nuvm_alloc(size_t size) {
	return malloc(size);
}

void* nuvm_alloc_unmanaged(size_t size) {
	return malloc(size);
}

void nuvm_free(void* ptr) {
	free(ptr);
}

void nuvm_init() {
	/* do nothing so far. */
}

