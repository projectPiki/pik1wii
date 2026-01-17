#include "DebugLog.h"
#include "RevoSDK/os.h"
#include "TAI/Amotion.h"
#include "system.h"
#include "teki.h"

/**
 * @todo: Documentation
 * @note UNUSED Size: 00009C
 */
DEFINE_ERROR(__LINE__) // Never used in the DLL

/**
 * @todo: Documentation
 * @note UNUSED Size: 0000F4
 */
DEFINE_PRINT("TAIAmotion")

/**
 * @todo: Documentation
 */
TAIAmotion::TAIAmotion(int nextState, int motionID)
    : TaiAction(nextState)
{
	mMotionID = motionID;
}

/**
 * @todo: Documentation
 */
void TAIAmotion::start(Teki& teki)
{
	PaniMotionInfo anim(mMotionID, &teki);
	teki.mTekiAnimator->startMotion(anim);
}

/**
 * @todo: Documentation
 */
bool TAIAmotion::act(Teki& teki)
{
	if (teki.mCurrentAnimEvent == KEY_Finished) {
		return true;
	}

	return false;
}

/**
 * @todo: Documentation
 */
TAIAreserveMotion::TAIAreserveMotion(int nextState, int motionID)
    : TaiAction(nextState)
{
	mMotionID = motionID;
}

/**
 * @todo: Documentation
 */
void TAIAreserveMotion::start(Teki& teki)
{
	if (mMotionID != teki.mTekiAnimator->mMotionIdx) {
		if (teki.mTekiAnimator->mMotionIdx < 0) {
			PaniMotionInfo anim1(mMotionID, &teki);
			teki.mTekiAnimator->startMotion(anim1);
			return;
		}

		if (teki.mTekiAnimator->getCounter() >= f32(teki.mTekiAnimator->mAnimInfo->mData->mTotalFrameCount - 2)) {
			PaniMotionInfo anim2(mMotionID, &teki);
			teki.mTekiAnimator->startMotion(anim2);
			return;
		}

		PaniMotionInfo anim3(PANI_NO_MOTION, &teki);
		teki.mTekiAnimator->finishMotion(anim3);
		return;
	}

	if (teki.mTekiAnimator->getCounter() >= f32(teki.mTekiAnimator->mAnimInfo->mData->mTotalFrameCount - 2)) {
		PaniMotionInfo anim4(mMotionID, &teki);
		teki.mTekiAnimator->startMotion(anim4);
	}

	if (teki.mTekiAnimator->mIsFinished) {
		f32 frame = teki.mTekiAnimator->getCounter();
		PaniMotionInfo anim5(mMotionID, &teki);
		teki.mTekiAnimator->startMotion(anim5);
		teki.mTekiAnimator->setCounter(frame);
	}
}

/**
 * @todo: Documentation
 */
bool TAIAreserveMotion::act(Teki& teki)
{
	if (mMotionID != teki.mTekiAnimator->mMotionIdx) {
		if (teki.mTekiAnimator->getCurrentKeyIndex() < 0) {
			PaniMotionInfo anim(mMotionID, &teki);
			teki.mTekiAnimator->startMotion(anim);
			teki.mCurrentAnimEvent = KEY_Reserved;
			return true;
		}
		return false;
	}

	return true;
}

/**
 * @todo: Documentation
 */
void TAIAmotionLoop::start(Teki& teki)
{
	TAIAreserveMotion::start(teki);
	teki.setFrameCounter(0.0f);
}

/**
 * @todo: Documentation
 */
bool TAIAmotionLoop::act(Teki& teki)
{
	bool res = false;
	if (TAIAreserveMotion::act(teki)) {
		teki.addFrameCounter(gsys->getFrameTime());
		if (teki.getFrameCounter() > getFrameMax(teki)) {
			res = true;
			PaniMotionInfo anim(PANI_NO_MOTION, &teki);
			teki.mTekiAnimator->finishMotion(anim);
		}
	}
	return res;
}
