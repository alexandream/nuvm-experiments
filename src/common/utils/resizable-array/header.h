#include "preamble.h"

#ifndef N_DS_ARRAY_P_SKIP_STRUCT
typedef struct {
	int32_t count;
	int32_t capacity;
	N_DS_ARRAY_CONTENTS_TYPE* elements;
} N_DS_ARRAY_TYPE_NAME;
#endif

#ifndef N_DS_ARRAY_P_SKIP_PROTOTYPES

N_DS_ARRAY_STATIC
void N_DS_ARRAY_INIT(N_DS_ARRAY_TYPE_NAME*array, int32_t size);


#ifndef N_DS_ARRAY_P_SKIP_DESTROY
N_DS_ARRAY_STATIC
void N_DS_ARRAY_DESTROY(N_DS_ARRAY_TYPE_NAME* array);
#endif


#ifndef N_DS_ARRAY_P_SKIP_APPEND
N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_APPEND(N_DS_ARRAY_TYPE_NAME* array, N_DS_ARRAY_CONTENTS_TYPE value);
#endif


N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_CAPACITY(N_DS_ARRAY_TYPE_NAME* array);


N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_COUNT(N_DS_ARRAY_TYPE_NAME* array);


#ifndef N_DS_ARRAY_P_SKIP_ELEMENTS
N_DS_ARRAY_STATIC
N_DS_ARRAY_CONTENTS_TYPE* N_DS_ARRAY_ELEMENTS(N_DS_ARRAY_TYPE_NAME* array);
#endif


N_DS_ARRAY_STATIC
N_DS_ARRAY_CONTENTS_TYPE N_DS_ARRAY_GET(N_DS_ARRAY_TYPE_NAME* array, int32_t index);


N_DS_ARRAY_STATIC
int32_t N_DS_ARRAY_RESIZE(N_DS_ARRAY_TYPE_NAME* array, int32_t size);


N_DS_ARRAY_STATIC
void N_DS_ARRAY_SET(N_DS_ARRAY_TYPE_NAME* array,
               int32_t index,
               N_DS_ARRAY_CONTENTS_TYPE value);

#endif /* N_DS_ARRAY_P_SKIP_PROTOTYPES */
#include "undefines.h"
