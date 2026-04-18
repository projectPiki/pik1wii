#include "System12/FrameCounter.h"
#include "egg/prim/eggAssert.h"
#include "egg/core/eggDvdRipper.h"
#include "RevoSDK/sc.h"
#include <string.h>

namespace System12 {

FrameCounter::FrameCounter() 
	: mCurrentFrame(0.0f)
	, mSpeed(0.0f)
	, mDiff(-1.0f)
	, mMaxFrames(0.0f)
	, mUserFrameRangeStart(0.0f)
	, mUserFrameRangeEnd(0.0f)
	, mType(TYPE_Unk0)
	, _20(0)
{
}

void FrameCounter::setCurrentFrame(f32 frame)
{
	mFlags.makeAllZero();
	mCurrentFrame = frame;
}

u16 FrameCounter::isEnd() const
{
	return mFlags.on(1);
}

u16 FrameCounter::isTurnedMax() const
{
	return mFlags.on(2);
}

void FrameCounter::setUserFrameRange(f32 start, f32 end)
{
	EGG_ASSERT(90, start >= 0);
	EGG_ASSERT(91, end <= mMaxFrames);
	mUserFrameRangeStart = start;
	mUserFrameRangeEnd = end;
}

void FrameCounter::resetUserFrameRange()
{
	mUserFrameRangeStart = 0.0f;
	mUserFrameRangeEnd = mMaxFrames;
}

void FrameCounter::play(FrameCounter::eType type, f32 frame)
{
	mType = type;
	mSpeed = frame;
	_20 = 0;
	mDiff = -1.0f;
	mFlags.makeAllZero();
	if (frame >= 0.0f) {
		mCurrentFrame = mUserFrameRangeStart;
		return;
	}
	mCurrentFrame = mUserFrameRangeEnd;
}

void FrameCounter::playFromCurrent(FrameCounter::eType type, f32 frame)
{
	mType = type;
	mSpeed = frame;
	_20 = 0;
	mDiff = -1.0f;
	mFlags.makeAllZero();
	if (mUserFrameRangeStart > mCurrentFrame && mCurrentFrame > mUserFrameRangeEnd) {
		return;
	}
	EGG_PRINT("out of range [%.1f] [%.1f-%.1f]\n", mCurrentFrame, mUserFrameRangeStart, mUserFrameRangeEnd);
}

void FrameCounter::playFromCurrentByDiff(FrameCounter::eType type, f32 frame, f32 diff)
{
	playFromCurrent(type, frame);
	mDiff = diff;
	_20 = 1;
	EGG_ASSERT(104, diff > 0.0f);
}

void FrameCounter::playFromCurrentToTrg(FrameCounter::eType type, f32 frame, f32 trg)
{
	playFromCurrent(type, frame);
	_20 = 1;
	if (frame > 0.0f) {
		mDiff = trg - getCurrentFrame();
	} else {
		mDiff = getCurrentFrame() - trg;
	}
	if (mDiff < 0.0f) {
		mDiff += mUserFrameRangeEnd - mUserFrameRangeStart;
	}
	EGG_ASSERT(122, trg >= 0.0f);
}

void FrameCounter::stopCurrent()
{
	mFlags.makeAllZero();
	mSpeed = 0.0f;
}

}