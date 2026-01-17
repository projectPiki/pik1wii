#ifndef _REVOSDK_VEC_H
#define _REVOSDK_VEC_H

#include "types.h"

BEGIN_SCOPE_EXTERN_C

typedef struct Vec {
	f32 x;
	f32 y;
	f32 z;
} Vec;

typedef struct SVec {
	s16 x;
	s16 y;
	s16 z;
} SVec;

f32 PSVECSquareMag(Vec*);
f32 PSVECMag(const Vec*);
void PSVECAdd(const Vec*, const Vec*, Vec*);
void PSVECSubtract(const Vec*, const Vec*, Vec*);
void PSVECNormalize(const Vec*, Vec*);
void PSVECCrossProduct(const Vec*, const Vec*, Vec*);

END_SCOPE_EXTERN_C

#endif
