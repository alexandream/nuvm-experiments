#ifndef __NUVM__LOADER_H__
#define __NUVM__LOADER_H__

#include "value.h"
#include "error.h"

NValue
n_load(const char* name, NError*);

#endif
