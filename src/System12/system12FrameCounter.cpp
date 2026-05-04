#include "RevoSDK/sc.h"
#include "System12/FrameCounter.h"
#include "egg/core/eggDvdRipper.h"
#include "egg/prim/eggAssert.h"
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
	mUserFrameRangeEnd   = end;
}

void FrameCounter::resetUserFrameRange()
{
	mUserFrameRangeStart = 0.0f;
	mUserFrameRangeEnd   = mMaxFrames;
}

void FrameCounter::play(FrameCounter::eType type, f32 frame)
{
	mType  = type;
	mSpeed = frame;
	_20    = 0;
	mDiff  = -1.0f;
	mFlags.makeAllZero();
	if (frame >= 0.0f) {
		mCurrentFrame = mUserFrameRangeStart;
		return;
	}
	mCurrentFrame = mUserFrameRangeEnd;
}

void FrameCounter::playFromCurrent(FrameCounter::eType type, f32 frame)
{
	mType  = type;
	mSpeed = frame;
	_20    = 0;
	mDiff  = -1.0f;
	mFlags.makeAllZero();
	if (!(mCurrentFrame < mUserFrameRangeStart) && !(mCurrentFrame > mUserFrameRangeEnd)) {
		return;
	}
	EGG_PRINT("out of range [%.1f] [%.1f-%.1f]\n", mCurrentFrame, mUserFrameRangeStart, mUserFrameRangeEnd);
}

void FrameCounter::playFromCurrentByDiff(FrameCounter::eType type, f32 frame, f32 diff)
{
	playFromCurrent(type, frame);
	mDiff = diff;
	_20   = 1;
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

void FrameCounter::stopAtEnd()
{
	mSpeed        = 0.0f;
	mCurrentFrame = mUserFrameRangeEnd;
	mFlags.makeAllZero();
	mFlags.set(1);
}

void FrameCounter::stop(f32 frame)
{
	mCurrentFrame = frame;
	mSpeed        = 0.0f;
	mFlags.makeAllZero();
	if (!(mCurrentFrame < mUserFrameRangeStart) && !(mCurrentFrame > mUserFrameRangeEnd)) {
		return;
	}
	EGG_PRINT("out of range [%.1f] [%.1f-%.1f]\n", mCurrentFrame, mUserFrameRangeStart, mUserFrameRangeEnd);
}

void FrameCounter::calc()
{
	mFlags.makeAllZero();
	switch (_20) {
	case 0:
		mCurrentFrame = mCurrentFrame + mSpeed;
		break;
	case 1:
		if (mDiff != 0.0f) {
			if (mDiff > 0.0f) {
				if (mDiff - mSpeed < 0.0f) {
					mCurrentFrame = mCurrentFrame + mDiff;
					mDiff         = 0.0f;
				} else {
					mDiff         = mDiff - (mSpeed > 0.0f ? mSpeed : -mSpeed);
					mCurrentFrame = mCurrentFrame + mSpeed;
				}
			} else {
				EGG_ASSERT_MSG(174, 0, "false");
			}
		}
		break;
	default:
		EGG_ASSERT_MSG(178, 0, "false");
		break;
	}

	if (mSpeed == 0.0f) {
		if (mCurrentFrame >= mUserFrameRangeEnd) {
			switch (mType) {
			case TYPE_Unk0:
				mCurrentFrame = mUserFrameRangeEnd;
				mFlags.set(1);
				break;
			case TYPE_Unk1:
				mFlags.set(2);
				mCurrentFrame = mUserFrameRangeStart + (mCurrentFrame - mUserFrameRangeEnd);
				break;
			case TYPE_Unk2:
				mFlags.set(2);
				mCurrentFrame = mUserFrameRangeEnd - (mCurrentFrame - mUserFrameRangeEnd);
				mSpeed *= -1.0f;
				break;
			}
		} else {
			switch (mType) {
			case TYPE_Unk0:
				mCurrentFrame = mUserFrameRangeStart;
				mFlags.set(1);
				break;
			case TYPE_Unk1:
				mCurrentFrame = mUserFrameRangeEnd;
				mFlags.set(4);
				break;
			case TYPE_Unk2:
				mFlags.set(4);
				mCurrentFrame = -mCurrentFrame + mUserFrameRangeStart;
				mSpeed *= -1.0f;
				break;
			}
		}
	}

	if (mSpeed > 0.0f) {
		if (mCurrentFrame >= mUserFrameRangeEnd) {
			switch (mType) {
			case TYPE_Unk0:
				mFlags.set(1);
				mCurrentFrame = mUserFrameRangeEnd;
				break;
			case TYPE_Unk1:
				mFlags.set(2);
				mCurrentFrame = mUserFrameRangeStart + (mCurrentFrame - mUserFrameRangeEnd);
				break;
			case TYPE_Unk2:
				mFlags.set(2);
				mCurrentFrame = mUserFrameRangeEnd - (mCurrentFrame - mUserFrameRangeEnd);
				mSpeed *= -1.0f;
				break;
			}
		}
	} else {
		if (mCurrentFrame < mUserFrameRangeStart) {
			switch (mType) {
			case TYPE_Unk0:
				mCurrentFrame = mUserFrameRangeStart;
				mFlags.set(1);
				break;
			case TYPE_Unk1:
				mCurrentFrame = mUserFrameRangeEnd;
				mFlags.set(4);
				break;
			case TYPE_Unk2:
				mFlags.set(4);
				mCurrentFrame = -mCurrentFrame + mUserFrameRangeStart;
				mSpeed *= -1.0f;
				break;
			}
		}
	}
}

} // namespace System12
