#include "DebugLog.h"
#include "PikiAI.h"

/**
 * @todo: Documentation
 * @note UNUSED Size: 00009C
 */
DEFINE_ERROR(__LINE__) // Never used in the DLL

/**
 * @todo: Documentation
 * @note UNUSED Size: 0000F4
 */
DEFINE_PRINT("aiRandomBoid")

/**
 * @todo: Documentation
 */
void ActRandomBoid::AnimListener::animationKeyUpdated(immut PaniAnimKeyEvent& event)
{
	switch (event.mEventType) {
	case KEY_Finished:
	{
		mAction->mIsAnimFinishing = false;
		switch (mAction->mState) {
		case STATE_Boid:
		{
			f32 angle = 2.0f * (PI * gsys->getRand(1.0f));
			if (gsys->getRand(1.0f) > 0.8f) {
				PaniMotionInfo anim1(PIKIANIM_Run, this);
				PaniMotionInfo anim2(PIKIANIM_Run);
				mPiki->startMotion(anim1, anim2);
				Vector3f vec(cosf(angle), 0.0f, sinf(angle));
				mPiki->setSpeed(-1.2f, vec);
				return;
			}

			if (gsys->getRand(1.0f) > 0.8f) {
				PaniMotionInfo anim1(PIKIANIM_Run, this);
				PaniMotionInfo anim2(PIKIANIM_Run);
				mPiki->startMotion(anim1, anim2);
				mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
				return;
			}

			PaniMotionInfo anim1(PIKIANIM_Run, this);
			PaniMotionInfo anim2(PIKIANIM_Run);
			mPiki->startMotion(anim1, anim2);
			Vector3f vec(cosf(angle), 0.0f, sinf(angle));
			mPiki->setSpeed(0.0f, vec);
			break;
		}
		case STATE_Stop:
		{
			PaniMotionInfo anim1(PIKIANIM_Run, this);
			PaniMotionInfo anim2(PIKIANIM_Run);
			mPiki->startMotion(anim1, anim2);
			break;
		}
		case STATE_Random:
		{
			PaniMotionInfo anim1(PIKIANIM_Run, this);
			PaniMotionInfo anim2(PIKIANIM_Run);
			mPiki->startMotion(anim1, anim2);
			f32 angle2 = 2.0f * (PI * gsys->getRand(1.0f));
			mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
			break;
		}
		}
		break;
	}
	case KEY_Action0:
	{
		break;
	}
	}
}

/**
 * @todo: Documentation
 */
ActRandomBoid::ActRandomBoid(Piki* piki)
    : Action(piki, true)
{
	mListener = new AnimListener(this, piki);
}

/**
 * @todo: Documentation
 */
void ActRandomBoid::Initialiser::initialise(Action*)
{
}

/**
 * @todo: Documentation
 */
void ActRandomBoid::init(Creature*)
{
	mState      = STATE_Boid;
	mStateTimer = int((10.0f * gsys->getRand(1.0f))) + 20;
	f32 angle   = 2.0f * (PI * gsys->getRand(1.0f));
	mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
	PaniMotionInfo anim1(PIKIANIM_Run, mListener);
	PaniMotionInfo anim2(PIKIANIM_Run, mListener);
	mPiki->startMotion(anim1, anim2);
	mListener->_0C   = false;
	mIsAnimFinishing = false;
}

/**
 * @todo: Documentation
 */
void ActRandomBoid::cleanup()
{
}

/**
 * @todo: Documentation
 */
int ActRandomBoid::exec()
{
	if (mIsAnimFinishing) {
		mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
		return ACTOUT_Continue;
	}

	if (--mStateTimer < 0) {
		mStateTimer    = int((12.0f * gsys->getRand(1.0f))) + 38;
		int startState = mState;
		if (mState == STATE_Boid && gsys->getRand(1.0f) > 0.5f) {
			mState = STATE_Idle;
			mPiki->mPikiAnimMgr.finishMotion(mListener);
			mIsAnimFinishing = true;
			mStateTimer += int((50.0f * gsys->getRand(1.0f))) + 30;
		} else if (gsys->getRand(1.0f) > 0.65f) {
			if (gsys->getRand(1.0f) > 0.75f) {
				mState = STATE_Random;
				if (startState != STATE_Random) {
					mPiki->mPikiAnimMgr.finishMotion(mListener);
					mIsAnimFinishing = true;
				}
			} else {
				mState = STATE_Boid;
				if (startState != STATE_Stop && startState != STATE_Boid) {
					mPiki->mPikiAnimMgr.finishMotion(mListener);
					mIsAnimFinishing = true;
				}
			}

			f32 randomAngle = 2.0f * (PI * gsys->getRand(1.0f));
			mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
			mStateTimer += 120;
		} else {
			mStateTimer += 120;
			mState = STATE_Stop;
			if (startState != STATE_Stop || startState != STATE_Boid) {
				mPiki->mPikiAnimMgr.finishMotion(mListener);
				mIsAnimFinishing = true;
				return ACTOUT_Continue;
			}
		}

		return ACTOUT_Continue;
	}

	if (mState == STATE_Stop) {
		mStateTimer = 1;
	}

	if (mState == STATE_Idle) {
		mPiki->mTargetVelocity.set(0.0f, 0.0f, 0.0f);
	}

	Vector3f avoidVec;
	if (mPiki->getAvoid(mPiki->mTargetVelocity, avoidVec)) {
		mPiki->mTargetVelocity = mPiki->mTargetVelocity + mPiki->getSpeed(0.5f) * avoidVec;
	}

	return ACTOUT_Continue;
}

static const char* stateName[] = {
	"ランダム", // 'random'
	"Boid",
	"停止", // 'stop'
};

/**
 * @todo: Documentation
 */
void ActRandomBoid::getInfo(char* buf)
{
	sprintf(buf, "%s", stateName[mState]);
}
