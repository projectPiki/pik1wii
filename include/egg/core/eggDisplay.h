#ifndef EGG_CORE_DISPLAY_H
#define EGG_CORE_DISPLAY_H

#include <egg/prim/eggBitFlag.h>

namespace EGG {

class Display {
public:
	enum EFlagBits { mFlag_SetClear, mFlag_WaitForRetrace };
	TBitFlag<u8> mEfbFlags; // _00
	enum EScreenStateBits { mScreenStateFlag_SetBlack };

public:
	virtual void beginFrame();     // _08
	virtual void beginRender();    // _0C
	virtual void endRender();      // _10
	virtual void endFrame();       // _14
	virtual u32 getTickPerFrame(); // _18

public:
	u8 mFrameRate;               // _08
	TBitFlag<u8> mXfbFlags;      // _09
	u32 mRetraceCount;           // _0C
	u32 mFrameCount;             // _10
	nw4r::ut::Color mClearColor; // _14
	u32 mClearZ;                 // _18
	s32 mPrevFrameTick;          // _1C
	s32 mFrameCostTick;          // _20
	f32 mFrequency;              // _24

public:
	Display(u8 maxRetrace);
	void copyEFBtoXFB();
	void calcFrequency();
	void setBlack(bool b) { }

	nw4r::ut::Color getClearColor() const { return mClearColor; }

	void setClearColor(nw4r::ut::Color color) { mClearColor = color; }

	u32 getClearZ() { return mClearZ; }

public:
	static u32 sTickPeriod;
};

} // namespace EGG

#endif
