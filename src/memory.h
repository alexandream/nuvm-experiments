#ifndef __NUVM__MEMORY_H__
#define __NUVM__MEMORY_H__

#include "util/types.h"

void* n_alloc_unmanaged(size_t size);
void* n_alloc(size_t size);

void n_free(void*);

#endif

