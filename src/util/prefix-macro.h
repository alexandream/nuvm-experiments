#ifndef __N__UTIL__PREFIX_MACRO_H__
#define __N__UTIL__PREFIX_MACRO_H__

#define EXP(V) V
#define CAT(P,V) P##V
#define EXP_CAT(P,V) CAT(P, V)

#define PREFIX_NAME(prefix, name) EXP_CAT(EXP_CAT(prefix,_), name)

#endif
