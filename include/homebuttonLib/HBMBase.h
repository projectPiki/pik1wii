#ifndef REVOSDK_HBM_BASE_INTERNAL_H
#define REVOSDK_HBM_BASE_INTERNAL_H

#include "homebuttonLib/HBMGUIManager.h"
#include "homebuttonLib/nw4hbm/lyt/drawInfo.h"
#include <RevoSDK/ax.h>
#include <RevoSDK/axfx.h>
#include <RevoSDK/hbm.h>
#include <RevoSDK/wpad.h>
#include <types.h>


namespace nw4hbm { 
namespace lyt { 
class ArcResourceAccessor;
class Layout;
class Pane; 
}
}
namespace homebutton { 
class Controller;
class GroupAnmController;
class RemoteSpk; 

class HomeButton;

class HomeButtonEventHandler : public gui::EventHandler {
public:
	HomeButtonEventHandler(HomeButton *pHomeButton)
		:mpHomeButton(pHomeButton)
		{	
		}

	virtual void onEvent(u32 uID, u32 uEvent, void *pData); // _08

	HomeButton *getHomeButton() { return mpHomeButton; }

private:
	HomeButton *mpHomeButton;	// _08
};

class HomeButton {
private:
	enum State
	{
		State0	= 0,
		State1	= 1,
		State2	= 2,
		State3	= 3,
		State4	= 4,
		State5	= 5,
		State6	= 6,
		State7	= 7,
		State8	= 8,
		State9	= 9,
		State10	= 10,
		State11	= 11,
		State12	= 12,
		State13	= 13,
		State14	= 14,
		State15	= 15,
		State16	= 16,
		State17	= 17,
		State18	= 18,
		State19	= 19
	};

	class BlackFader {
	public:
		enum State
		{
			StateNone,
			StateForward,
			StateBackward
		};

		BlackFader(int maxFrame)
		{
			init(maxFrame);
			setColor(0, 0, 0);
			flag = true;
		}

		int getFrame() const { return frame_; }
		GXColor getColor() { return (GXColor){red_, green_, blue_, 255}; }
		int getMaxFrame() const { return maxFrame_; }
		int getFadeColorEnable() const { return flag; }

		void setFadeColorEnable(bool a) { flag = a; }
		void setColor(u8 r, u8 g, u8 b)
		{
			red_ = r;
			green_ = g;
			blue_ = b;
		}

		bool isDone();

		void start() { state_ = StateForward; }

		void init(int maxFrame);
		void calc();
		void draw();

	private:
		int frame_;		// _00
		int maxFrame_;	// _04
		int state_;		// _08
		bool flag;		// _0C
		u8 red_;		// _0D
		u8 green_;		// _0E
		u8 blue_;		// _0F
	};

public:
	HomeButton(HBMDataInfo const *pHBInfo);
		
	~HomeButton();

	HBMDataInfo const *getHBMDataInfo() { return mpHBInfo; }
	Controller *getController(int chan) { return mpController[chan]; }
	int getVolume();
	HBMSelectBtnNum getSelectBtnNum();
	char const *getFuncPaneName(int no) { return scFuncTouchPaneName[no]; }
	char const *getPaneName(int no) { return scBtnName[no]; }
	bool getReassignedFlag() const { return mReassignedFlag; }
	HomeButtonEventHandler *getEventHandler() const
	{
		return mpHomeButtonEventHandler;
	}

	bool isActive() const;
	bool isUpBarActive() const;
	bool isDownBarActive();

	void setAdjustFlag(BOOL flag);
	void setEndSimpleSyncFlag(bool flag) { mEndSimpleSyncFlag = flag; }
	void setForcusSE();
	void setReassignedFlag(bool flag) { mReassignedFlag = flag; }
	void setSimpleSyncAlarm(int type);
	void setSimpleSyncFlag(bool flag) { mSimpleSyncFlag = flag; }
	void setSpeakerAlarm(int chan, int msec);
	void setVolume(int vol);

	bool getVibFlag();
	int getPaneNo(nw4hbm::lyt::Pane const *);

	void setVibFlag(bool flag);

	void create();
	void init();
	void calc(HBMControllerData const *pController);
	void draw();
	void update(HBMControllerData const *pController);
	void updateTrigPane();

	void startPointEvent(nw4hbm::lyt::Pane const *pPane, void *pData);
	void startLeftEvent(nw4hbm::lyt::Pane const *pPane);
	void startTrigEvent(nw4hbm::lyt::Pane const *pPane);

	int findAnimator(int pane, int anm);
	int findGroupAnimator(int pane, int anm);

	void callSimpleSyncCallback(s32 result, s32 num);

	void startBlackOut();
	void setBlackOutColor(u8 red, u8 green, u8 blue);
	GXColor getBlackOutColor();
	void setVIBlack(BOOL flag);

	static void createInstance(HBMDataInfo const *pHBInfo);
	static HomeButton *getInstance();
	static void deleteInstance();

private:
	void init_battery(HBMControllerData const *pController);
	void calc_battery(int chan);
	void reset_battery();

	void init_sound();
	void play_sound(int id);
	void fadeout_sound(f32 gain);

	void init_msg();
	void init_vib();
	void init_volume();

	void set_config();
	void set_text();

	void calc_fadeoutAnm();
	void calc_letter();

	void update_controller(int id);
	void update_posController(int id);

	void reset_btn();
	void reset_control();
	void reset_guiManager(int num);
	void reset_window();

public:
	void update_sound();

private:
	static HomeButton *spHomeButtonObj;
	static OSMutex sMutex;
	static WPADInfo sWpadInfo[WPAD_MAX_CONTROLLERS];

	/* Provided by HBMBase.inl in the source file. */
	static int const scSoundHeapSize_but2;
	static int const scSoundHeapSize_but3;
	static int const scSoundThreadPrio;
	static int const scDvdThreadPrio;

	static int const scReConnectTime;
	static int const scReConnectTime2;
	static int const scPadDrawWaitTime;
	static int const scGetPadInfoTime;
	static int const scForcusSEWaitTime;
	static f32 const scOnPaneVibTime;
	static f32 const scOnPaneVibWaitTime;
	static int const scWaitStopMotorTime;
	static int const scWaitDisConnectTime;

	static char const *scCursorLytName[WPAD_MAX_CONTROLLERS];
	static char const *scCursorPaneName;
	static char const *scCursorRotPaneName;
	static char const *scCursorSRotPaneName;

	static char const *scBtnName[4];
	static char const *scTxtName[4];
	static char const *scGrName[8];
	static char const *scAnimName[3];
	static char const *scPairGroupAnimName[15];
	static char const *scPairGroupName[15];
	static char const *scGroupAnimName[22];
	static char const *scGroupName[35];
	static char const *scFuncPaneName[5];
	static char const *scFuncTouchPaneName[10];
	static char const *scFuncTextPaneName[3];
	static char const *scBatteryPaneName[WPAD_MAX_CONTROLLERS][4];

private:
	enum
	{
		eSeq_Normal,
		eSeq_Control,
		eSeq_Cmn
	} mSequence; 												// _00
	HBMDataInfo const *mpHBInfo;								// _04
	HBMDataInfoEx const *mpHBInfoEx;							// _08
	int mButtonNum;												// _0C
	int mAnmNum;												// _10
	int mState;													// _14
	int mSelectAnmNum;											// _18
	int mMsgCount;												// _1C
	int mPaneCounter[14];										// _20
	int mPadDrawTime[WPAD_MAX_CONTROLLERS];						// _58
	int mForcusSEWaitTime;										// _68
	int mBar0AnmRev;											// _6C
	int mBar1AnmRev;											// _70
	int mBar0AnmRevHold;										// _74
	int mBar1AnmRevHold;										// _78
	int mGetPadInfoTime;										// _7C
	bool mControllerFlag[WPAD_MAX_CONTROLLERS];					// _80
	int mVolumeNum;												// _84
	int mConnectNum;											// _88
	bool mVibFlag;												// _8C
	bool mControlFlag;											// _8D
	bool mLetterFlag;											// _8E
	bool mAdjustFlag;											// _8F
	bool mReassignedFlag;										// _90
	bool mSimpleSyncFlag;										// _91
	bool mEndSimpleSyncFlag;									// _92
	bool mForthConnectFlag;										// _93
	bool mInitFlag;												// _94
	bool mForceSttInitProcFlag;									// _95
	bool mForceSttFadeInProcFlag;								// _96
	bool mEndInitSoundFlag;										// _97
	bool mForceStopSyncFlag;									// _98
	bool mForceEndMsgAnmFlag;									// _99
	bool mStartBlackOutFlag;									// _9A
	int mSoundRetryCnt;											// _9C
	BOOL mDialogFlag[4];										// _A0
	char *mpLayoutName;											// _B0
	char *mpAnmName;											// _B4
	HBMSelectBtnNum mSelectBtnNum;								// _B8
	wchar_t *mpText[10][6];										// _BC
	WPADInfo mWpadInfo[WPAD_MAX_CONTROLLERS];					// _1C0
	WPADSimpleSyncCallback mSimpleSyncCallback;					// _20C
	f32 mOnPaneVibFrame[4];										// _210
	f32 mOnPaneVibWaitFrame[4];									// _220
	int mWaitStopMotorCount;									// _230
	int mDisConnectCount;										// _234
	nw4hbm::lyt::Layout *mpLayout;								// _238
	nw4hbm::lyt::Layout *mpCursorLayout[WPAD_MAX_CONTROLLERS];	// _23C
	nw4hbm::lyt::ArcResourceAccessor *mpResAccessor;			// _24C
	gui::PaneManager *mpPaneManager;							// _250
	HomeButtonEventHandler *mpHomeButtonEventHandler;			// _254
	nw4hbm::lyt::DrawInfo mDrawInfo;							// _258
	Controller *mpController[WPAD_MAX_CONTROLLERS];				// _2AC
	RemoteSpk *mpRemoteSpk;										// _2BC
	GroupAnmController *mpAnmController[12];					// _2C0
	GroupAnmController *mpGroupAnmController[74];				// _2F0
	GroupAnmController *mpPairGroupAnmController[15];			// _418
	BlackFader mFader;											// _458
	OSAlarm mAlarm[WPAD_MAX_CONTROLLERS];						// _468
	OSAlarm mSpeakerAlarm[WPAD_MAX_CONTROLLERS];				// _528
	OSAlarm mSimpleSyncAlarm;									// _5E8
	BOOL iVISetBlackFlag;										// _5B8
	int iReConnectTime;											// _5BC
	int iReConnectTime2;										// _5C0
	u16 mAppVolume[3];											// _624
	u16 mBatteryCheck;											// _62A
	AXFXAllocHook mAxFxAlloc;									// _62C
	AXFXFreeHook mAxFxFree;										// _630
	AXFX_REVERBHI mAxFxReverb;									// _634
	AXAuxCallback mAuxCallback;									// _794
	void *mpAuxContext;											// _798
	f32 mFadeOutSeTime;											// _79C
};
} // namespace homebutton

#endif // REVOSDK_HBM_BASE_INTERNAL_H
