#ifndef ARRAY_PREAMBLE
#define ARRAY_PREAMBLE

#include <assert.h>
#include <stdlib.h>

#include "../../polyfills/p-stdint.h"
#include "../prefix-macro.h"

#define ARRAY_STATIC

#ifdef ARRAY_USE_STATIC
#define ARRAY_STATIC static
#endif

#ifndef ARRAY_TYPE_NAME
#error "ARRAY_TYPE_NAME must be defined to use the generic resizable array template."
#endif

#ifndef ARRAY_CONTENTS_TYPE
#error "ARRAY_CONTENTS_TYPE must be defined to use the generic resizable array template."
#endif


#ifndef ARRAY_ALLOCATOR
#define ARRAY_ALLOCATOR malloc
#endif

#ifndef ARRAY_COPY_ELEMENT
#define ARRAY_COPY_ELEMENT(E1, E2) *E1 = *E2
#endif

#ifndef ARRAY_DEALLOCATOR
#define ARRAY_DEALLOCATOR free
#endif

#ifndef ARRAY_PREFIX
#define ARRAY_PREFIX ARRAY_TYPE_NAME
#endif

#ifndef ARRAY_REALLOCATOR
#define ARRAY_REALLOCATOR realloc
#endif

#define ARRAY_INIT     NI_PREFIX_NAME(ARRAY_PREFIX, init)
#define ARRAY_DESTROY  NI_PREFIX_NAME(ARRAY_PREFIX, destroy)

#define ARRAY_APPEND   NI_PREFIX_NAME(ARRAY_PREFIX, append)
#define ARRAY_CAPACITY NI_PREFIX_NAME(ARRAY_PREFIX, capacity)
#define ARRAY_COUNT    NI_PREFIX_NAME(ARRAY_PREFIX, count)
#define ARRAY_ELEMENTS NI_PREFIX_NAME(ARRAY_PREFIX, elements)
#define ARRAY_GET      NI_PREFIX_NAME(ARRAY_PREFIX, get)
#define ARRAY_RESIZE   NI_PREFIX_NAME(ARRAY_PREFIX, resize)
#define ARRAY_SET      NI_PREFIX_NAME(ARRAY_PREFIX, set)

#endif
