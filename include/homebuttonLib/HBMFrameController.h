#ifndef REVOSDK_HBM_FRAME_CONTROLLER_H
#define REVOSDK_HBM_FRAME_CONTROLLER_H

#include <types.h>

namespace homebutton {
class FrameController {
public:
	enum eState
	{
		eState_Stopped,
		eState_Playing
	};

	enum eAnmType
	{
		eAnmType_Forward,
		eAnmType_Backward,
		eAnmType_Wrap,
		eAnmType_Alternate,

		eAnmType_Max
	};

	virtual ~FrameController() {}
	virtual void calc();

	f32 getMaxFrame() const { return mMaxFrame; }
	f32 getCurrentFrame() const { return mCurFrame; }
	f32 getLastFrame() const { return mMaxFrame - 1.0f; }
	bool isPlaying() const { return mState == eState_Playing; }

	void setAnmType(int anm_type) { mAnmType = anm_type; }

	void init(int anm_type, f32 max_frame, f32 min_frame, f32 delta);
	void initFrame();

	void start()
	{
		initFrame();
		restart();
	}

	void stop() { mState = eState_Stopped; }
	void restart() { mState = eState_Playing; }

protected:
	// _00 = VTBL
	f32 mMaxFrame;		// _04
	f32 mMinFrame;		// _08
	f32 mCurFrame;		// _0C
	f32 mFrameDelta;	// _10
	int mState;			// _14
	int mAnmType;		// _18
	bool mAltFlag;		// _1C
};
} // namespace homebutton

#endif // REVOSDK_HBM_FRAME_CONTROLLER_H
