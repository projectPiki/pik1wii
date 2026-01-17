#ifndef _REVOSDK_OS_OSFASTCAST_H
#define _REVOSDK_OS_OSFASTCAST_H

#include "RevoSDK/OS/OSUtil.h"
#include "types.h"

BEGIN_SCOPE_EXTERN_C

/////// FAST CAST DEFINES ////////
// GQR formats.
#define OS_GQR_U8  (0x0004) // GQR 1
#define OS_GQR_U16 (0x0005) // GQR 2
#define OS_GQR_S8  (0x0006) // GQR 3
#define OS_GQR_S16 (0x0007) // GQR 4

// GQRs for fast casting.
#define OS_FASTCAST_U8  (2)
#define OS_FASTCAST_U16 (3)
#define OS_FASTCAST_S8  (4)
#define OS_FASTCAST_S16 (5)

//////////////////////////////////

/////// FAST CAST INLINES ////////


/// f32 to u16
static inline u16 __OSf32tou16(register f32 arg)
{
	f32 a;
	register f32* ptr = &a;
	u16 r;

#ifdef __MWERKS__ // clang-format off
    asm {
        psq_st arg, 0(ptr), 1, 3
	}
#endif // clang-format on

	r = *(u16*)ptr;
	return r;
}
static inline void OSf32tou16(f32* in, volatile u16* out)
{
	*out = __OSf32tou16(*in);
}

// f32 to int.
// NB: should theoretically have these for u8/u16/s8/s16 eventually.
static inline s16 __OSf32tos16(register f32 inF)
{
	register s16 out;
	u32 tmp;
	register u32* tmpPtr = &tmp;
#ifdef __MWERKS__
	asm {
		psq_st    inF, 0 (tmpPtr), 0x1, OS_FASTCAST_S16
		lha       out, 0 (tmpPtr)
	}
#endif
	return out;
}

static inline void OSf32tos16(f32* f, s16* out)
{
	*out = __OSf32tos16(*f);
}

static inline u8 __OSf32tou8(register f32 inF)
{
	register u8 out;
	u32 tmp;
	register u32* tmpPtr = &tmp;
#ifdef __MWERKS__
	asm {
		psq_st    inF, 0 (tmpPtr), 0x1, OS_FASTCAST_U8
		lbz       out, 0 (tmpPtr)
	}
#endif
	return out;
}

static inline void OSf32tou8(f32* f, u8* out)
{
	*out = __OSf32tou8(*f);
}



// Int to f32.
// NB: should have these for u8/u16/s8/s16 eventually.

// TODO: make these based on above/as necessary.

/// Int to float.
/// u8 to f32
static inline f32 __OSu8tof32(register u8* arg)
{
	register f32 ret;

#ifdef __MWERKS__ // clang-format off
    asm {
        psq_l ret, 0(arg), 1, 2
	}
#endif // clang-format on

	return ret;
}
static inline void OSu8tof32(u8* in, volatile f32* out)
{
	*out = __OSu8tof32(in);
}

/// u16 to f32
static inline f32 __OSu16tof32(register u16* arg)
{
	register f32 ret;

#ifdef __MWERKS__ // clang-format off
    asm {
        psq_l ret, 0(arg), 1, 3
	}
#endif // clang-format on

	return ret;
}
static inline void OSu16tof32(u16* in, volatile f32* out)
{
	*out = __OSu16tof32(in);
}

/// s8 to f32
/// TODO

/// s16 to f32
static f32 __OSs16tof32(register s16* arg)
{
	register f32 ret;
#ifdef __MWERKS__ // clang-format off
    ASM (
        psq_l ret, 0(arg), 1, 5
    );
#endif // clang-format on

	return ret;
}
static void OSs16tof32(s16* in, volatile f32* out)
{
	*out = __OSs16tof32(in);
}

//////////////////////////////////

END_SCOPE_EXTERN_C

#endif
