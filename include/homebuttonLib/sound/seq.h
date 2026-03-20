#ifndef REVOSDK_HBMSEQ_H
#define REVOSDK_HBMSEQ_H

#include "homebuttonLib/sound/syn.h"
#include <types.h>

// Modifiable; to what extent is unknown
#define HBMSEQ_MAX_TRACKS	64

#ifdef __cplusplus
	extern "C" {
#endif

typedef u32 HBMSEQSTATE;
enum HBMSEQSTATE_et
{
	HBM_SEQ_STATE_0, // off?
	HBM_SEQ_STATE_1, // on?
	HBM_SEQ_STATE_2, // some sort of next?
	HBM_SEQ_STATE_3, // paused?
};

// forward declarations
typedef struct _HBMSEQSEQUENCE HBMSEQSEQUENCE;


typedef struct _HBMSEQTRACK
{
	HBMSEQSEQUENCE *sequence;	// _00
	u8 *start;					// _04
	u8 *end;					// _08
	u8 *current;				// _0C
	u8 status;					// _10
	f32 beatsPerSec;			// _14
	u32 defaultTicksPerFrame;	// _18
	u32 ticksPerFrame;			// _1C
	u32 delay;					// _20
	u32 state;					// _24
} HBMSEQTRACK;

struct _HBMSEQSEQUENCE
{
	HBMSEQSEQUENCE *next;					// _00
	HBMSEQSTATE state;						// _04
	u16 nTracks;							// _08
	s16 timeFormat;							// _0A
	u32 tracksRunning;						// _0C
	u32 end;								// _10
	HBMSYNSYNTH synth;						// _14
	HBMSEQTRACK track[HBMSEQ_MAX_TRACKS];	// _241C
};

void HBMSEQInit(void);
void HBMSEQQuit(void);

void HBMSEQRunAudioFrame(void);

void HBMSEQAddSequence(HBMSEQSEQUENCE *sequence, u8 *midiStream,
                       u8 *wavetable, u8 *samples, u8 *zerobuffer);
void HBMSEQRemoveSequence(HBMSEQSEQUENCE *sequence);

void HBMSEQSetState(HBMSEQSEQUENCE *sequence, HBMSEQSTATE state);
HBMSEQSTATE HBMSEQGetState(HBMSEQSEQUENCE *sequence);

void HBMSEQSetVolume(HBMSEQSEQUENCE *sequence, s32 dB);
s32 HBMSEQGetVolume(HBMSEQSEQUENCE *sequence);

#ifdef __cplusplus
	}
#endif

#endif // REVOSDK_HBMSEQ_H