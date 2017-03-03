#ifndef __NUVM__NUVM_H__
#define __NUVM__NUVM_H__

#include "util/common.h"
#include "value.h"


extern const uint8_t nuvm_version[];

void* nuvm_alloc_unmanaged(size_t size);
void* nuvm_alloc(size_t size);
void nuvm_free(void* ptr);
void nuvm_init();

#endif

