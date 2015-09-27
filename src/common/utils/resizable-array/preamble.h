#ifndef N_DS_ARRAY_PREAMBLE
#define N_DS_ARRAY_PREAMBLE

#include <assert.h>
#include <stdlib.h>

#include "../../polyfills/p-stdint.h"
#include "../prefix-macro.h"

#endif


#ifdef N_DS_ARRAY_USE_STATIC
  #ifndef N_DS_ARRAY_STATIC
    #define N_DS_ARRAY_STATIC static
  #endif
#else
  #define N_DS_ARRAY_STATIC
#endif

#ifndef N_DS_ARRAY_TYPE_NAME
#error "N_DS_ARRAY_TYPE_NAME must be defined to use the generic resizable array template."
#endif

#ifndef N_DS_ARRAY_CONTENTS_TYPE
#error "N_DS_ARRAY_CONTENTS_TYPE must be defined to use the generic resizable array template."
#endif


#ifndef N_DS_ARRAY_ALLOCATOR
#define N_DS_ARRAY_ALLOCATOR malloc
#endif

#ifndef N_DS_ARRAY_COPY_ELEMENT
#define N_DS_ARRAY_COPY_ELEMENT(E1, E2) *E1 = *E2
#endif

#ifndef N_DS_ARRAY_DEALLOCATOR
#define N_DS_ARRAY_DEALLOCATOR free
#endif

#ifndef N_DS_ARRAY_PREFIX
#define N_DS_ARRAY_PREFIX N_DS_ARRAY_TYPE_NAME
#endif

#ifndef N_DS_ARRAY_REALLOCATOR
#define N_DS_ARRAY_REALLOCATOR realloc
#endif

#define N_DS_ARRAY_INIT     NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, init)
#define N_DS_ARRAY_DESTROY  NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, destroy)

#define N_DS_ARRAY_APPEND   NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, append)
#define N_DS_ARRAY_CAPACITY NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, capacity)
#define N_DS_ARRAY_COUNT    NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, count)
#define N_DS_ARRAY_DETACH   NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, detach)
#define N_DS_ARRAY_ELEMENTS NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, elements)
#define N_DS_ARRAY_GET      NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, get)
#define N_DS_ARRAY_RESIZE   NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, resize)
#define N_DS_ARRAY_SET      NI_PREFIX_NAME(N_DS_ARRAY_PREFIX, set)

