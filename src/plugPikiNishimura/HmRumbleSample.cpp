#include "RevoSDK/wpad.h"
#include "RumbleMgr.h"

/**
 * @todo: Documentation
 */
RumbleSample::RumbleSample(int chan)
{
	mCurrentIntensity = 0.0f;
	mTotalIntensity   = 0.0f;
	mChannel          = chan;
}

/**
 * @todo: Documentation
 * @note UNUSED Size: 000010
 */
void RumbleSample::init()
{
	// UNUSED FUNCTION
}

/**
 * @todo: Documentation
 */
void RumbleSample::simpleStop()
{
	WPADStopMotor(mChannel);
}

/**
 * @todo: Documentation
 */
void RumbleSample::simpleStart(f32 intensity)
{
	mCurrentIntensity = intensity;
	mTotalIntensity += mCurrentIntensity;
	if (mCurrentIntensity <= 0.0f) {
		WPADStopMotor(mChannel);

	} else if (mTotalIntensity < 1.0f) {
		WPADStopMotor(mChannel);

	} else {
		mTotalIntensity -= 1.0f;
		WPADStartMotor(mChannel);
	}
}
