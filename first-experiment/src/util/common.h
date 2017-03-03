#ifndef __NUVM__UTILS__COMMON_H__
#define __NUVM__UTILS__COMMON_H__

#define EXP(V) V
#define CAT(P,V) P##V
#define EXP_CAT(P,V) CAT(P, V)

#define PREFIX_NAME(prefix, name) EXP_CAT(EXP_CAT(prefix,_), name)

#include <stddef.h>
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>



#endif
