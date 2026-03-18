#include "homebuttonLib/HBMRemoteSpk.h"
#include <RevoSDK/arc.h>
#include <RevoSDK/wenc.h>
#include <RevoSDK/wpad.h>
#include <types.h>


namespace homebutton {

RemoteSpk *RemoteSpk::spInstance;


void RemoteSpk::SetInstance(RemoteSpk *pThis)
{
	spInstance = pThis;
}

RemoteSpk *RemoteSpk::GetInstance()
{
	return spInstance;
}

void RemoteSpk::GetPCMFromSeID(int in_ID, s16 *&out_wave,
                               int &out_length)
{
	ARCFileInfo af;
	ARCFastOpen(&handle, in_ID, &af);

	out_wave = static_cast<s16 *>(ARCGetStartAddrInMem(&af));
	out_length = static_cast<int>(ARCGetLength(&af));

	ARCClose(&af);
}

static bool MakeVolumeData(s16 const *src, s16 *dst,
                           int vol, u32 size)
{
	u32 enc_size = MIN_EQ(size, 40);
	for (int i = 0; i < enc_size; ++i)
		*dst++ = static_cast<s16>(*src++ * vol / 10);

	if (size > 40)
		return false;

	u32 zero_size = 40 - size;
	for (int i = 0; i < zero_size; ++i)
		*dst++ = 0;

	return true;
}

void RemoteSpk::UpdateSpeaker(OSAlarm *, OSContext *)
{
	s16 pcmBuffer[40];
	u8 adpcmBuffer[20];

	if (!GetInstance())
		return;

	ChanInfo *pinfo = GetInstance()->info;

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++, pinfo++)
	{
		if (pinfo->in_pcm && WPADIsSpeakerEnabled(i))
		{
			BOOL intrStatus = OSDisableInterrupts();

			if (WPADCanSendStreamData(i))
			{
				MakeVolumeData(pinfo->in_pcm, pcmBuffer, pinfo->vol,
				               pinfo->length / sizeof(s16));
				WENCGetEncodeData(&pinfo->wencinfo, pinfo->first ? 0 : 1,
				                  pcmBuffer, ARRAY_SIZE(pcmBuffer),
				                  adpcmBuffer);
				WPADSendStreamData(i, adpcmBuffer, ARRAY_SIZE(adpcmBuffer));

				pinfo->first = false;
				pinfo->cannotSendCnt = 0;
				pinfo->in_pcm += ARRAY_SIZE(pcmBuffer);
				pinfo->length -= sizeof pcmBuffer;

				if (pinfo->length <= 0)
				{
					pinfo->seId = -1;
					pinfo->in_pcm = nullptr;
				}
			}
			else
			{
				++pinfo->cannotSendCnt;

				if (pinfo->cannotSendCnt > 60)
					pinfo->in_pcm = nullptr;
			}

			OSRestoreInterrupts(intrStatus);
		}
	}
}

void RemoteSpk::ClearPcm()
{
	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		info[i].in_pcm = nullptr;
		info[i].seId = -1;
	}
}

RemoteSpk::RemoteSpk(void *spkSeBuf)
{
	SetInstance(this);

	if (spkSeBuf)
		available = ARCInitHandle(spkSeBuf, &handle);
	else
		available = false;

	OSCreateAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		OSCreateAlarm(&info[i].alarm);
		info[i].in_pcm = nullptr;
		info[i].seId = -1;
		info[i].first = true;
		info[i].playReady = true;
	}
}

RemoteSpk::~RemoteSpk()
{
	SetInstance(nullptr);
	available = false;

	OSCancelAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
		OSCancelAlarm(&info[i].alarm);
}

void RemoteSpk::Start()
{
	if (!available)
		return;

	OSCreateAlarm(&speakerAlarm);
	OSSetPeriodicAlarm(&speakerAlarm, OSGetTime(),
	                   OSNanosecondsToTicks(6666667), &UpdateSpeaker);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		OSCreateAlarm(&info[i].alarm);

		info[i].in_pcm = nullptr;
		info[i].seId = -1;
		info[i].first = true;
		info[i].playReady = true;
	}
}

void RemoteSpk::Stop()
{
	OSCancelAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
		OSCancelAlarm(&info[i].alarm);
}

void RemoteSpk::DelaySpeakerOnCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);
	s32 result =
		WPADControlSpeaker(chan, WPAD_SPEAKER_ENABLE, &SpeakerOnCallback);
}

void RemoteSpk::SpeakerOnCallback(WPADChannel chan, s32 result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ERR_NONE:
		pRmtSpk->info[chan].first = true;
		result =
			WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
		break;

	case WPAD_ERR_TRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_ERR_BUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerOnCallback);
		break;
	}
}

void RemoteSpk::DelaySpeakerOffCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);
	s32 result = WPADControlSpeaker(chan, WPAD_SPEAKER_DISABLE, &SpeakerOffCallback);
}

void RemoteSpk::SpeakerOffCallback(WPADChannel chan, s32 result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ERR_NONE:
	case WPAD_ERR_TRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_ERR_BUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerOffCallback);
		break;
	}
}


void RemoteSpk::DelaySpeakerPlayCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);

	s32 result =
		WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
}

void RemoteSpk::SpeakerPlayCallback(WPADChannel chan, s32 result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ERR_NONE:
		pRmtSpk->info[chan].playReady = true;
		break;

	case WPAD_ERR_TRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_ERR_NO_CONTROLLER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_ERR_BUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerPlayCallback);
		break;
	}
}

void RemoteSpk::Connect(WPADChannel chan)
{
	if (!available)
		return;

	int result = WPADControlSpeaker(chan, WPAD_SPEAKER_ENABLE, &SpeakerOnCallback);

	u32* p = reinterpret_cast<u32*>(&info[chan].wencinfo);

	for (int i = sizeof (WENCInfo) / sizeof(u32); i > 0;
	     i--, p++)
	{
		*p = 0;
	}

	info[chan].first = true;
	info[chan].playReady = false;
}

void RemoteSpk::Disconnect(WPADChannel chan)
{
	if (!available)
		return;

	int result = WPADControlSpeaker(chan, WPAD_SPEAKER_DISABLE, &SpeakerOffCallback);

	info[chan].playReady = false;
}

void RemoteSpk::Play(WPADChannel chan, int seID, s8 vol)
{
	if (!available)
		return;

	s16 *pcm;
	int length;
	GetPCMFromSeID(seID, pcm, length);

	info[chan].cannotSendCnt = 0;
	info[chan].seId = seID;
	info[chan].length = length;
	info[chan].vol = vol;
	info[chan].in_pcm = pcm;
}

bool RemoteSpk::isPlaying(WPADChannel chan) const
{
	return info[chan].in_pcm != nullptr;
}

bool RemoteSpk::isPlayingId(WPADChannel chan, int seId) const
{
	if (isPlaying(chan) && info[chan].seId == seId)
		return true;
	else
		return false;
}

bool RemoteSpk::isPlayReady(WPADChannel chan) const
{
	return info[chan].playReady != false;
}

} // namespace homebutton
