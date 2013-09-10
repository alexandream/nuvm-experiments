#ifndef __NUVM__NUVM_H__
#define __NUVM__NUVM_H__

#include "util/common.h"

extern const uint8_t nuvm_version[];

void nuvm_init();
void* nuvm_alloc_unmanaged(size_t size);

#endif

