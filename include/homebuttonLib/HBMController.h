#ifndef REVOSDK_HBM_CONTROLLER_H
#define REVOSDK_HBM_CONTROLLER_H

#include <RevoSDK/hbm.h>
#include <RevoSDK/wpad.h>
#include <types.h>



namespace homebutton { 
class RemoteSpk; 

class Controller {
public:
	struct HBController {
		int chan;		// _00
		f32 spVol;		// _04
		f32 x;			// _08
		f32 y;			// _0c
		u32 trig;		// _10
		u32 hold;		// _14
		u32 release;	// _18
		bool rumble;	// _1C
	}; 

	Controller(int chan, RemoteSpk *spk);
		
	~Controller();

	HBController *getController();
	int getChan() const;
	f32 getSpeakerVol() const;
	RemoteSpk *getRemoteSpk() const { return remotespk; }
	bool isRumbling() { return mHBController.rumble; }
	bool getBatteryFlag() const;

	void setSpeakerVol(f32 vol);
	void setEnableRumble(bool flag) { mRumbleFlag = flag; }
	void setRumble() { mHBController.rumble = true; }
	void clrRumble() { mHBController.rumble = false; }

	s32 getInfoAsync(WPADInfo *info);
	bool isPlayReady() const;
	bool isPlayingSound() const;
	bool isPlayingSoundId(int id) const;

	void setKpad(HBMKPadData const *con, bool updatePos);
	void setInValidPos();
	void clrBatteryFlag();
	void clrKpadButton();

	void connect();
	void disconnect();

	void initSound();
	void updateSound();
	void playSound(int id);

	void soundOn();
	void soundOff(int msec);

	void startMotor();
	void stopMotor();

	void initCallback();
	void clearCallback();

private:
	static void wpadConnectCallback(WPADChannel chan, s32 result);
	static void wpadExtensionCallback(WPADChannel chan, s32 result);
	static void soundOnCallback(OSAlarm *alm, OSContext *context);
	static void ControllerCallback(WPADChannel chan, s32 result);

	static bool sBatteryFlag[WPAD_MAX_CONTROLLERS];
	static OSAlarm sAlarm[WPAD_MAX_CONTROLLERS];
	static OSAlarm sAlarmSoundOff[WPAD_MAX_CONTROLLERS];
	static Controller *sThis[WPAD_MAX_CONTROLLERS];
	static bool sSetInfoAsync[WPAD_MAX_CONTROLLERS];

	HBController mHBController;						// _00
	RemoteSpk *remotespk;							// _20
	WPADConnectCallback mOldConnectCallback;		// _24
	WPADConnectCallback mOldExtensionCallback;		// _28
	OSTime mPlaySoundTime;							// _30
	OSTime mStopSoundTime;							// _38
	bool mCallbackFlag;								// _40
	bool mSoundOffFlag;								// _41
	bool mCheckSoundTimeFlag;						// _42
	bool mCheckSoundIntervalFlag;					// _43
	bool mRumbleFlag;								// _44
	};
} // namespace homebutton

#endif // REVOSDK_HBM_CONTROLLER_H
