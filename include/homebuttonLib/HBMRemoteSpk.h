#ifndef REVOSDK_HBM_REMOTE_SPK_H
#define REVOSDK_HBM_REMOTE_SPK_H

#include <RevoSDK/arc.h>
#include <RevoSDK/wenc.h>
#include <RevoSDK/wpad.h>
#include <types.h>

namespace homebutton {
class RemoteSpk {
private:
	struct ChanInfo {
		OSAlarm alarm;		// _00
		WENCInfo wencinfo;	// _30
		s16 const *in_pcm;	// _50
		int length;			// _54
		int seId;			// _58
		bool first;			// _5C
		s8 vol;				// _5D
		s8 cannotSendCnt;	// _5E
		u16 pad; 			// _60
		bool playReady;		// _62
	}; 

public:
	RemoteSpk(void *spkSeBuf);
	
	bool isPlayReady(WPADChannel chan) const;
	bool isPlaying(WPADChannel chan) const;
	bool isPlayingId(WPADChannel chan, int seId) const;

	void GetPCMFromSeID(int in_ID, s16 *&out_wave, int &out_length);
	void ClearPcm();
	void Start();
	void Stop();
	void Connect(WPADChannel chan);
	void Disconnect(WPADChannel chan);
	void Play(WPADChannel chan, int seID, s8 vol);

	static void SetInstance(RemoteSpk *pThis);
	static RemoteSpk *GetInstance();

private:
	static void UpdateSpeaker(OSAlarm *alarm, OSContext *context);

	static void SpeakerOnCallback(WPADChannel chan, s32 result);
	static void DelaySpeakerOnCallback(OSAlarm *alarm, OSContext *context);

	static void SpeakerOffCallback(WPADChannel chan, s32 result);
	static void DelaySpeakerOffCallback(OSAlarm *alarm, OSContext *context);

	static void SpeakerPlayCallback(WPADChannel chan, s32 result);
	static void DelaySpeakerPlayCallback(OSAlarm *alarm, OSContext *context);

	static RemoteSpk *spInstance;

	ChanInfo info[WPAD_MAX_CONTROLLERS];	// _00
	OSAlarm speakerAlarm;					// _1A0
	ARCHandle handle;						// _1D0
	bool available;							// _1EC

public:
	virtual ~RemoteSpk();
};
} // namespace homebutton

#endif // REVOSDK_HBM_REMOTE_SPK_H
