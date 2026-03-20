#ifndef REVOSDK_HBMSYN_H
#define REVOSDK_HBMSYN_H

#include <types.h>

#include <RevoSDK/ax.h>

#define HBMSYN_NUM_MIDI_CHANNELS	16
#define HBMSYN_NUM_MIDI_NOTES		128

// Modifiable; to what extent is unknown
#define HBMSYN_INPUT_BUFFER_SIZE	256

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct HBMSYNVOICE HBMSYNVOICE;
typedef struct HBMSYNSYNTH HBMSYNSYNTH;

typedef struct WTADPCM
{
	u16	a[8][2];			// _00
	u16	gain;				// _20
	u16	pred_scale;			// _22
	u16	yn1;				// _24
	u16	yn2;				// _26
	u16	loop_pred_scale;	// _28
	u16	loop_yn1;			// _2A
	u16	loop_yn2;			// _2C
} WTADPCM; 

typedef struct WTART
{
	s32	lfoFreq;		// _00
	s32	lfoDelay;		// _04
	s32	lfoAtten;		// _08
	s32	lfoPitch;		// _0C
	s32	lfoMod2Atten;	// _10
	s32	lfoMod2Pitch;	// _14
	s32	eg1Attack;		// _18
	s32	eg1Decay;		// _1C
	s32	eg1Sustain;		// _20
	s32	eg1Release;		// _24
	s32	eg1Vel2Attack;	// _28
	s32	eg1Key2Decay;	// _2C
	s32	eg2Attack;		// _30
	s32	eg2Decay;		// _34
	s32	eg2Sustain;		// _38
	s32	eg2Release;		// _3C
	s32	eg2Vel2Attack;	// _40
	s32	eg2Key2Decay;	// _44
	s32	eg2Pitch;		// _48
	s32	pan;			// _4C
} WTART; // size 0x50

typedef struct WTINST
{
	u16	keyRegion[HBMSYN_NUM_MIDI_NOTES];
} WTINST;

typedef struct WTREGION
{
	u8	unityNote;			// _00
	u8	keyGroup;			// _01
	s16	fineTune;			// _02
	s32	attn;				// _04
	u32	loopStart;			// _08
	u32	loopLength;			// _0c
	u32	articulationIndex;	// _10
	u32	sampleIndex;		// _14
} WTREGION;

typedef struct WTSAMPLE
{
	u16	format;		// _00
	u16	sampleRate;	// _02
	u32	offset;		// _04
	u32	length;		// _08
	u16	adpcmIndex;	// _0C
} WTSAMPLE;

struct HBMSYNSYNTH
{
	HBMSYNSYNTH	*next;														// _00
	WTINST *percussiveInst;													// _04
	WTINST *melodicInst;													// _08
	WTREGION *region;														// _0C
	WTART *art;																// _10
	WTSAMPLE *sample;														// _14
	WTADPCM *adpcm;															// _18
	u32 samplesBaseWord;													// _1C
	u32 samplesBaseByte;													// _20
	u32 samplesBaseNibble;													// _24
	WTINST *inst[HBMSYN_NUM_MIDI_CHANNELS];									// _28
	s32 masterVolume;														// _68
	s32 volAttn[HBMSYN_NUM_MIDI_CHANNELS];									// _6C
	s32 auxAAttn[HBMSYN_NUM_MIDI_CHANNELS];									// _AC
	u8 pan[HBMSYN_NUM_MIDI_CHANNELS];										// _EC
	u8 input[HBMSYN_INPUT_BUFFER_SIZE][3];									// _FC
	u8 *inputPosition;														// _3FC
	u32 inputCounter;														// _400
	u32 notes;																// _404
	HBMSYNVOICE	*voice[HBMSYN_NUM_MIDI_CHANNELS][HBMSYN_NUM_MIDI_NOTES];	// _408
};

struct HBMSYNVOICE
{
	void *next;				// _00
	AXVPB *axvpb;			// _04
	HBMSYNSYNTH	*synth;		// _08
	u8 midiChannel;			// _0C
	u8 keyNum;				// _0D
	u8 keyVel;				// _0E
	u8 pan;					// _0F
	WTREGION *region;		// _10
	WTART *art;				// _14
	WTSAMPLE *sample;		// _18
	WTADPCM *adpcm;			// _1C
	u32 type;				// _20
	f32 srcRatio;			// _24
	s32 cents;				// _28
	s32 attn;				// _2C
	u32 veState;			// _30
	s32 veAttn;				// _34
	s32 veAttack;			// _38
	s32 veAttackDelta;		// _3C
	s32 veDecay;			// _40
	s32 veSustain;			// _44
	s32 veRelease;			// _48
};

enum VolumeEnvelopeStages
{
	VolEnvState_Attack,
	VolEnvState_Decay,
	VolEnvState_Sustain,
	VolEnvState_Release,

	VolEnvState_End
};

extern f32 __HBMSYNn128[];
extern s32 __HBMSYNVolumeAttenuation[];
extern s32 __HBMSYNAttackAttnTable[];
extern HBMSYNVOICE *__HBMSYNVoice;


void HBMSYNInit(void);
void HBMSYNQuit(void);

void HBMSYNRunAudioFrame(void);

void HBMSYNInitSynth(HBMSYNSYNTH *synth, u8 *wavetable, u8 *samples,
                     u8 *zerobuffer);
void HBMSYNQuitSynth(HBMSYNSYNTH *synth);

void HBMSYNMidiInput(HBMSYNSYNTH *synth, u8 *input);

void HBMSYNSetMasterVolume(HBMSYNSYNTH *synth, s32 dB);
s32 HBMSYNGetMasterVolume(HBMSYNSYNTH *synth);

void __HBMSYNSetController(HBMSYNSYNTH *synth, u8 midiChannel, u8 function,
                           u8 value);
void __HBMSYNResetAllControllers(HBMSYNSYNTH *synth);
void __HBMSYNRunInputBufferEvents(HBMSYNSYNTH *synth);
BOOL __HBMSYNGetWavetableData(HBMSYNVOICE *voice);


s32 __HBMSYNGetEnvelopeTime(s32 scale, s32 mod, u8 key);
void __HBMSYNSetupVolumeEnvelope(HBMSYNVOICE *voice);
void __HBMSYNRunVolumeEnvelope(HBMSYNVOICE *voice);

void __HBMSYNSetupVolume(HBMSYNVOICE *voice);
void __HBMSYNSetupPan(HBMSYNVOICE *voice);
s32 __HBMSYNGetVoiceInput(HBMSYNVOICE *voice);
s32 __HBMSYNGetVoiceFader(HBMSYNVOICE *voice);
void __HBMSYNUpdateMix(HBMSYNVOICE *voice);

f32 __HBMSYNGetRelativePitch(HBMSYNVOICE *voice);
void __HBMSYNSetupPitch(HBMSYNVOICE *voice);
void __HBMSYNSetupSrc(HBMSYNVOICE *voice);
void __HBMSYNUpdateSrc(HBMSYNVOICE *voice);

void __HBMSYNSetupSample(HBMSYNVOICE *voice);

void __HBMSYNClearVoiceReferences(void *p);
void __HBMSYNSetVoiceToRelease(HBMSYNVOICE *voice);
void __HBMSYNServiceVoice(int i);
#ifdef __cplusplus
	}
#endif

#endif // REVOSDK_HBMSYN_H