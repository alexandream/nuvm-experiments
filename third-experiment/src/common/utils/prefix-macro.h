#ifndef NHG_U_PREFIX_MACRO
#define NHG_U_PREFIX_MACRO

#define NI_MACRO_CAT(P,V) P##V
#define NI_EXPAND_CAT(P,V) NI_MACRO_CAT(P, V)

#define NI_PREFIX_NAME(prefix, name)\
	NI_EXPAND_CAT(NI_EXPAND_CAT(prefix,_), name)

#endif /* NHG_U_PREFIX_MACRO */
