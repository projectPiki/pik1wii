#include "homebuttonLib/sound/syn.h"
#include "homebuttonLib/sound/mix.h"

#include <RevoSDK/OS.h>
#include <types.h>

static HBMSYNSYNTH *__HBMSYNSynthList;
HBMSYNVOICE *__HBMSYNVoice;
static HBMSYNVOICE __s_HBMSYNVoice[16];
static int __init;


static void __HBMSYNAddSynthToList(HBMSYNSYNTH *synth)
{
	BOOL intrStatus = OSDisableInterrupts();

	if (__HBMSYNSynthList)
		synth->next = __HBMSYNSynthList;
	else
		synth->next = nullptr;

	__HBMSYNSynthList = synth;

	OSRestoreInterrupts(intrStatus);
}

static void __HBMSYNRemoveSynthFromList(HBMSYNSYNTH *synth)
{
	HBMSYNSYNTH *tempHead, *tempTail;
	HBMSYNSYNTH *tempSynth;

	BOOL intrStatus = OSDisableInterrupts();

	tempHead = nullptr;
	tempTail = nullptr;

	for (tempSynth = __HBMSYNSynthList; tempSynth; tempSynth = tempSynth->next)
	{
		if (tempSynth != synth)
		{
			if (tempHead)
			{
				tempTail->next = tempSynth;
				tempTail = tempSynth;
			}
			else
			{
				tempTail = tempSynth;
				tempHead = tempTail;
			}
		}
	}

	if (tempTail)
		tempTail->next = nullptr;

	__HBMSYNSynthList = tempHead;

	OSRestoreInterrupts(intrStatus);

	(void)tempSynth; // ok
}

void HBMSYNInit(void)
{
	int i;

	if (AXIsInit() && !__init)
	{
		__HBMSYNVoice = __s_HBMSYNVoice;

		for (i = 0; i < (int)ARRAY_SIZE(__s_HBMSYNVoice); i++)
			__HBMSYNVoice[i].synth = nullptr;

		__HBMSYNSynthList = nullptr;
		__init = TRUE;
	}
}

void HBMSYNQuit(void)
{
	__HBMSYNVoice = nullptr;
	__init = FALSE;
}

void HBMSYNRunAudioFrame(void)
{
	int i;
	HBMSYNSYNTH *synth;

	if (!__init)
		return;

	for (i = 0; i < (int)ARRAY_SIZE(__s_HBMSYNVoice); i++)
		__HBMSYNServiceVoice(i);

	for (synth = __HBMSYNSynthList; synth; synth = synth->next)
		__HBMSYNRunInputBufferEvents(synth);
}

void HBMSYNInitSynth(HBMSYNSYNTH *synth, byte_t *wavetable, byte_t *samples,
                     byte_t *zerobuffer)
{
	u32 *p;
	u32 mramBase;
	u32 midiChannel, noteNumber;

	p = (u32 *)wavetable;
	mramBase = (u32)OSCachedToPhysical(samples);

	// Two full expressions per line
	synth->percussiveInst = (WTINST *)(wavetable + *p); p++;
	synth->melodicInst = (WTINST *)(wavetable + *p); p++;
	synth->region = (WTREGION *)(wavetable + *p); p++;
	synth->art = (WTART *)(wavetable + *p); p++;
	synth->sample = (WTSAMPLE *)(wavetable + *p); p++;
	synth->adpcm = (WTADPCM *)(wavetable + *p); p++;

	synth->samplesBaseWord = mramBase >> 1;
	synth->samplesBaseByte = mramBase;
	synth->samplesBaseNibble = mramBase << 1;
	synth->masterVolume = 0;

	__HBMSYNResetAllControllers(synth);

	synth->inputPosition = *synth->input;
	synth->inputCounter = 0;
	synth->notes = 0;

	for (midiChannel = 0; midiChannel < HBMSYN_NUM_MIDI_CHANNELS; midiChannel++)
	{
		for (noteNumber = 0; noteNumber < HBMSYN_NUM_MIDI_NOTES; noteNumber++)
			synth->voice[midiChannel][noteNumber] = nullptr;
	}

	__HBMSYNAddSynthToList(synth);
}

void HBMSYNQuitSynth(HBMSYNSYNTH *synth)
{
	int i;
	BOOL intrStatus = OSDisableInterrupts();

	if (synth->notes)
	{
		for (i = 0; i < (int)ARRAY_SIZE(__s_HBMSYNVoice); i++)
		{
			HBMSYNVOICE *voice = __HBMSYNVoice + i;

			if (voice->synth == synth)
			{
				HBMMIXReleaseChannel(voice->axvpb);
				HBMFreeIndexByKey(voice->axvpb->index);
				AXFreeVoice(voice->axvpb);
				voice->synth = nullptr;
			}
		}
	}

	__HBMSYNRemoveSynthFromList(synth);

	OSRestoreInterrupts(intrStatus);
}

void HBMSYNMidiInput(HBMSYNSYNTH *synth, byte_t *input)
{
	byte_t *src;

	src = input;

	// Three full expressions per line
	*synth->inputPosition = *src; synth->inputPosition++; src++;
	*synth->inputPosition = *src; synth->inputPosition++; src++;
	*synth->inputPosition = *src; synth->inputPosition++; src++;

	synth->inputCounter++;
}

void HBMSYNSetMasterVolume(HBMSYNSYNTH *synth, s32 dB)
{
	synth->masterVolume = dB << 16;
}
