#include "homebuttonLib/sound/syn.h"

#include "homebuttonLib/sound/mix.h"
#include <RevoSDK/AX.h>
#include <types.h>

void __HBMSYNClearVoiceReferences(void *p)
{
	AXVPB *axvpb;
	HBMSYNSYNTH *synth;
	HBMSYNVOICE *voice;
	int index;
	
	axvpb = (AXVPB *)p;
	synth = (HBMSYNSYNTH *)axvpb->userContext;

	index = HBMGetIndex(axvpb->index);
	HBMFreeIndex(index);

	voice = __HBMSYNVoice + index;
	HBMMIXReleaseChannel(axvpb);

	if (synth->voice[voice->midiChannel][voice->keyNum] == voice)
		synth->voice[voice->midiChannel][voice->keyNum] = nullptr;

	voice->synth = nullptr;
	synth->notes--;
}

void __HBMSYNSetVoiceToRelease(HBMSYNVOICE *voice)
{
	voice->veState = VolEnvState_Release;
}

void __HBMSYNServiceVoice(int i)
{
	HBMSYNVOICE *voice = __HBMSYNVoice + i;
	HBMSYNSYNTH *synth = voice->synth;

	if (!synth)
		return;

	if (voice->type == 0 && voice->axvpb->pb.state == 0)
	{
		if (synth->voice[voice->midiChannel][voice->keyNum] == voice)
			synth->voice[voice->midiChannel][voice->keyNum] = nullptr;

		voice->veState = VolEnvState_End;
	}

	__HBMSYNRunVolumeEnvelope(voice);

	if (voice->veState == VolEnvState_End)
	{
		voice->synth = nullptr;

		HBMMIXReleaseChannel(voice->axvpb);
		HBMFreeIndexByKey(voice->axvpb->index);
		AXFreeVoice(voice->axvpb);

		synth->notes--;
	}
	else
	{
		__HBMSYNUpdateMix(voice);
		__HBMSYNUpdateSrc(voice);
	}
}
