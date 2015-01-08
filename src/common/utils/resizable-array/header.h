#include "preamble.h"

#ifndef ARRAY__STRUCT__SKIP
typedef struct {
	int32_t count;
	int32_t capacity;
	ARRAY_CONTENTS_TYPE* elements;
} ARRAY_TYPE_NAME;
#endif


ARRAY_STATIC
void ARRAY_INIT(ARRAY_TYPE_NAME*array, int32_t size);


#ifndef ARRAY_DESTROY__SKIP
ARRAY_STATIC
void ARRAY_DESTROY(ARRAY_TYPE_NAME* array);
#endif


#ifndef ARRAY_APPEND__SKIP
ARRAY_STATIC
int32_t ARRAY_APPEND(ARRAY_TYPE_NAME* array, ARRAY_CONTENTS_TYPE value);
#endif


ARRAY_STATIC
int32_t ARRAY_CAPACITY(ARRAY_TYPE_NAME* array);


ARRAY_STATIC
int32_t ARRAY_COUNT(ARRAY_TYPE_NAME* array);


#ifndef ARRAY_ELEMENTS__SKIP
ARRAY_STATIC
ARRAY_CONTENTS_TYPE* ARRAY_ELEMENTS(ARRAY_TYPE_NAME* array);
#endif


ARRAY_STATIC
ARRAY_CONTENTS_TYPE ARRAY_GET(ARRAY_TYPE_NAME* array, int32_t index);


ARRAY_STATIC
int32_t ARRAY_RESIZE(ARRAY_TYPE_NAME* array, int32_t size);


ARRAY_STATIC
void ARRAY_SET(ARRAY_TYPE_NAME* array,
               int32_t index,
               ARRAY_CONTENTS_TYPE value);

#include "undefines.h"
