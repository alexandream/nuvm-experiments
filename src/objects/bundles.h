#ifndef __NUVM__OBJECTS__BUNDLES_H_
#define __NUVM__OBJECTS__BUNDLES_H_
#include <stddef.h>

#include "../error.h"
#include "../value.h"

typedef struct NBundle NBundle;

void
n_init_bundles();

NBundle*
n_bundle_new(uint16_t size, NError*);

void
n_bundle_close(NBundle*);

void
n_bundle_set(NBundle*, NValue symbol, NValue value, NError*);

uint16_t
n_bundle_size(NBundle*);

#endif
