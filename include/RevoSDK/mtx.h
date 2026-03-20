#ifndef _REVOSDK_MTX_H
#define _REVOSDK_MTX_H

#include "types.h"

#include "RevoSDK/vec.h"

BEGIN_SCOPE_EXTERN_C

/////////////// TYPE DEFINES ///////////////
#define MTXDegToRad(a) ((a) * 0.01745329252f)

typedef f32 Mtx[3][4];
typedef f32 Mtx23[2][3];
typedef f32 Mtx33[3][3];
typedef f32 Mtx44[4][4];
typedef f32 (*MtxP)[4];
typedef f32 (*Mtx3P)[3];
typedef f32 PSQuaternion[4];

typedef struct Quaternion {
	f32 x, y, z, w;
} Quaternion;

////////////////////////////////////////////

////// PAIRED SINGLE MATRIX FUNCTIONS //////
void PSMTXIdentity(Mtx mtx);
void PSMTXCopy(const Mtx src, Mtx dest);
void PSMTXConcat(const Mtx A, const Mtx B, Mtx concat);
void PSMTXConcatArray(const Mtx, const Mtx, Mtx, u32);

void PSMTXTranspose(const Mtx src, Mtx xPose);
u32 PSMTXInverse(const Mtx src, Mtx inv);
u32 PSMTXInvXpose(const Mtx src, Mtx xPose);

void PSMTXRotRad(Mtx mtx, char axis, f32 angle);
void PSMTXRotTrig(Mtx mtx, char axis, f32 sinA, f32 cosA);
void PSMTXRotAxisRad(Mtx mtx, const Vec* axis, f32 angle);

void PSMTXTrans(Mtx mtx, f32 xT, f32 yT, f32 zT);
void PSMTXTransApply(const Mtx src, Mtx dest, f32 xT, f32 yT, f32 zT);

void PSMTXScale(Mtx mtx, f32 xS, f32 yS, f32 zS);
void PSMTXScaleApply(const Mtx src, Mtx dest, f32 xS, f32 yS, f32 zS);
void PSMTXQuat(Mtx mtx, const PSQuaternion* quat);

////////////////////////////////////////////

//// PAIRED SINGLE MATRIX VEC FUNCTIONS ////
void PSMTXMultVec(const Mtx, const Vec*, Vec*);
void PSMTXMultVecSR(const Mtx, const Vec*, Vec*);
void PSMTXMultVecArraySR(const Mtx, f32*, f32*, f32*);

////////////////////////////////////////////

///////////  MATRIX44 FUNCTIONS ////////////
void PSMTX44Copy(Mtx44 src, Mtx44 dest);
void C_MTXFrustum(Mtx44 m, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6);
void C_MTXPerspective(Mtx44 mtx, f32 fovY, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void PSMTX44MultVec(const Mtx44 m, const Vec* src, Vec* dst);
////////////////////////////////////////////

///////// CODED C MATRIX FUNCTIONS /////////
void C_MTXLookAt(Mtx, const Vec*, const Vec*, const Vec*);
void C_MTXLightPerspective(Mtx mtx, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
void C_MTXLightFrustum(Mtx m, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9);
void C_MTXLightOrtho(Mtx mtx, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
////////////////////////////////////////////

///////// QUAT FUNCTIONS /////////
void PSQUATMultiply(const Quaternion* a, const Quaternion* b, Quaternion* prod);
void PSQUATNormalize(const Quaternion* in, Quaternion* out);
void C_QUATMtx(Quaternion* quat, const Mtx mtx);
void C_QUATSlerp(const Quaternion* a, const Quaternion* b, Quaternion* out, f32 t);

void MTXTrans(Mtx m, f32 xT, f32 yT, f32 zT);
void MTXScale(Mtx m, f32 xS, f32 yS, f32 zS);
void MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
void MTXPerspective(Mtx44 mtx, f32 fovY, f32 aspect, f32 n, f32 f);
void MTXOrtho(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

////////////// MATRIX INLINES //////////////
static inline void MTXSetPosition(Mtx mtx, const Vec* pos)
{
	mtx[0][3] = pos->x;
	mtx[1][3] = pos->y;
	mtx[2][3] = pos->z;
}

////////////////////////////////////////////

END_SCOPE_EXTERN_C

#endif
