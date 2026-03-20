#ifndef NW4HBM_MATH_TYPES_H
#define NW4HBM_MATH_TYPES_H

#include <RevoSDK/mtx.h>
#include <types.h>

namespace nw4hbm {
namespace math {

struct _VEC2 {
    f32 x;	// _00
    f32 y;	// _04
};

struct _VEC3 {
    f32 x;	// _00
    f32 y;	// _04
    f32 z;	// _08
};

struct _MTX34 {
    union {
        struct {
            f32 _00;	// _00
            f32 _01;	// _04
            f32 _02;	// _08
            f32 _03;	// _0C
            f32 _10;	// _10
            f32 _11;	// _14
            f32 _12;	// _18
            f32 _13;	// _1C
            f32 _20;	// _20
            f32 _21;	// _24
            f32 _22;	// _28
            f32 _23;	// _2C
        };

        f32 m[3][4];
        f32 a[12];
        Mtx mtx;
    };	// _00
};

struct VEC2 : public _VEC2 {
public:
    VEC2() {}
    VEC2(f32 fx, f32 fy) {
        x = fx;
        y = fy;
    }

    // operators
    operator f32*() { return reinterpret_cast<f32*>(this); }
    operator const f32*() const { return reinterpret_cast<const f32*>(this); }

public:
};

struct VEC3 : public _VEC3 {
public:
    VEC3() {}
    VEC3(f32 fx, f32 fy, f32 fz) {
        x = fx;
        y = fy;
        z = fz;
    }

    // operators
    operator Vec*() { return reinterpret_cast<Vec*>(this); }
	operator const Vec*() const { return reinterpret_cast<const Vec*>(this); }

public:
};

typedef f32 (*MtxPtr)[4];
typedef const f32 (*CMtxPtr)[4];

struct MTX34 : public _MTX34 {
public:
    MTX34() {} // NOTE: must be non-defaulted

    // operators
    operator MtxPtr() { return mtx; }
    operator CMtxPtr() const { return mtx; }

public:
};

inline MTX34* MTX34Copy(MTX34* pOut, const MTX34* p) {
    PSMTXCopy(*(Mtx*)p, *(Mtx*)pOut);
    return pOut;
}

inline MTX34* MTX34Mult(MTX34* pOut, const MTX34* p1, const MTX34* p2) {
    PSMTXConcat(*(Mtx*)p1, *(Mtx*)p2, *(Mtx*)pOut);
    return pOut;
}

inline MTX34* MTX34Identity(MTX34* pOut) {
    PSMTXIdentity(*(Mtx*)pOut);
    return pOut;
}

} // namespace math
} // namespace nw4hbm

#endif // NW4HBM_MATH_TYPES_H
