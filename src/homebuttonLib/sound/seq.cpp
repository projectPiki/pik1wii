#include "homebuttonLib/sound/seq.h"

#include "homebuttonLib/sound/syn.h"
#include <RevoSDK/OS.h>
#include <types.h>


#define MIDI_TRACK_HEADER_MAGIC					'MThd'
#define MIDI_TRACK_MAGIC						'MTrk'

#define MIDI_MESSAGE_CHANNEL_MODE_MESSAGE		0xB0
#define MIDI_MESSAGE_SYSEX_BEGIN				0xF0
#define MIDI_MESSAGE_SYSEX_END					0xF7
#define MIDI_MESSAGE_META_EVENT					0xFF

#define MIDI_CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF	0x7B

#define MIDI_META_EVENT_END_OF_TRACK			0x2F
#define MIDI_META_EVENT_SET_TEMPO				0x51

static u8 __HBMSEQMidiEventLength[128] =
{
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

	0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
// clang-format on

static HBMSEQSEQUENCE *__HBMSEQSequenceList;
static BOOL __init;

static void __HBMSEQPushSequenceList(HBMSEQSEQUENCE *sequence)
{
	BOOL intrStatus = OSDisableInterrupts();

	if (__HBMSEQSequenceList)
		sequence->next = __HBMSEQSequenceList;
	else
		sequence->next = nullptr;

	__HBMSEQSequenceList = sequence;

	OSRestoreInterrupts(intrStatus);
}

static void __HBMSEQRemoveSequenceFromList(HBMSEQSEQUENCE *sequence)
{
	BOOL intrStatus = OSDisableInterrupts();
	HBMSEQSEQUENCE *thisSequence = __HBMSEQSequenceList;

	__HBMSEQSequenceList = nullptr;

	while (thisSequence)
	{
		HBMSEQSEQUENCE *next = thisSequence->next;

		if (thisSequence != sequence)
			__HBMSEQPushSequenceList(thisSequence);

		thisSequence = next;
	}

	OSRestoreInterrupts(intrStatus);
}

static u32 __HBMSEQGetIntTrack(HBMSEQTRACK *track)
{
	u32 value;

	value = *track->current & 0x7f;

	while (*track->current & 0x80)
	{
		track->current++;

		value = (value << 7) + (*track->current & 0x7f);
	}

	track->current++;
	return value;
}

static void __HBMSEQHandleSysExEvent(HBMSEQTRACK *track)
{
	u32 length;

	length = __HBMSEQGetIntTrack(track);
	track->current += length;
}

static void __HBMSEQSetTicksPerFrame(HBMSEQTRACK *track, f32 bps)
{
	HBMSEQSEQUENCE *sequence;

	sequence = track->sequence;

	track->beatsPerSec = bps;
	track->ticksPerFrame =
		96.0f / (32000.0f / bps / sequence->timeFormat) * 65536.0f;
}

static void __HBMSEQTempoMetaEvent(HBMSEQTRACK *track)
{
	u32 data;
	f32 beatsPerSec;

	data = *track->current; track->current++;
	data = (data << 8) + *track->current; track->current++;
	data = (data << 8) + *track->current; track->current++;

	beatsPerSec = 1000000.0f / data;

	__HBMSEQSetTicksPerFrame(track, beatsPerSec);
}

static void __HBMSEQTrackEnd(HBMSEQTRACK *track)
{
	HBMSEQSEQUENCE *sequence;

	sequence = track->sequence;
	sequence->tracksRunning--;
	track->state = 0;

	if (!sequence->tracksRunning)
		sequence->end = 1;
}

static void __HBMSEQHandleMetaEvent(HBMSEQTRACK *track)
{
	byte_t type;
	u32 length;

	type = *track->current; track->current++;
	switch (type)
	{
	case MIDI_META_EVENT_END_OF_TRACK:
		__HBMSEQTrackEnd(track);
		break;

	case MIDI_META_EVENT_SET_TEMPO:
		length = __HBMSEQGetIntTrack(track);
		__HBMSEQTempoMetaEvent(track);
		break;

	default:
		length = __HBMSEQGetIntTrack(track);
		track->current += length;
		break;
	}
}

static void __HBMSEQHandleSynthEvent(HBMSYNSYNTH *synth, HBMSEQTRACK *track)
{
	byte_t ch[3];
	u32 bytes;

	bytes = __HBMSEQMidiEventLength[track->status - 128];
	ch[0] = track->status;

	switch (bytes)
	{
	case 0:
		break;

	case 1:
		ch[1] = *track->current; track->current++;
		break;

	case 2:
		ch[1] = *track->current; track->current++;
		ch[2] = *track->current; track->current++;
		break;
	}

	HBMSYNMidiInput(synth, ch);
}

static void __HBMSEQRunEvent(HBMSYNSYNTH *synth, HBMSEQTRACK *track) DECOMP_DONT_INLINE
{
	byte_t event;

	event = *track->current;
	if (event >= 0x80)
	{
		track->status = event;
		track->current++;
	}

	switch (track->status)
	{
	case MIDI_MESSAGE_SYSEX_BEGIN:
	case MIDI_MESSAGE_SYSEX_END:
		__HBMSEQHandleSysExEvent(track);
		break;

	case MIDI_MESSAGE_META_EVENT:
		__HBMSEQHandleMetaEvent(track);
		break;

	default:
		__HBMSEQHandleSynthEvent(synth, track);
		break;
	}

	if (track->current >= track->end)
		__HBMSEQTrackEnd(track);
}

static void __HBMSEQInitTracks(HBMSEQSEQUENCE *sequence, byte_t *read,
                               int tracks)
{
	int i = 0;
	byte_t *p = read;

	for (; tracks; tracks--, (void)i++)
	{
		byte4_t chunk;
		u32 bytes;

	next_chunk:
		chunk = *(byte4_t *)p; p += sizeof(byte4_t);
		bytes = *(u32 *)p; p += sizeof(u32);

		if (chunk == MIDI_TRACK_MAGIC)
		{
			HBMSEQTRACK *track = sequence->track + i;

			track->sequence = sequence;
			track->start = p;
			track->end = p + bytes;
			track->current = p;
			track->defaultTicksPerFrame =
				96.0f / (16000.0f / sequence->timeFormat) * 65536.0f;
			track->state = 0;

			p += bytes;
			continue;
		}
		else
		{
			p += bytes;
			goto next_chunk;
		}

	}
}

static void __HBMSEQReadHeader(HBMSEQSEQUENCE *sequence, byte_t *midiStream)
{
	byte_t *read = midiStream;
	u32 bytes;
	u32 fileType;

	read += sizeof(u32);

	bytes = *(u32 *)read; read += sizeof(u32);
	fileType = *(u16 *)read; read += sizeof(u16);
	sequence->nTracks = *(u16 *)read; read += sizeof(u16);
	sequence->timeFormat = *(s16 *)read; read += sizeof(s16);

	// the fields for fileType, sequence->nTracks, and sequence->timeFormat
	bytes -= sizeof(u16) + sizeof(u16) + sizeof(s16);
	read += bytes;

	switch (fileType)
	{
	case 0:
		sequence->nTracks = 1;
		__HBMSEQInitTracks(sequence, read, 1);
		break;
	case 1:
		__HBMSEQInitTracks(sequence, read, sequence->nTracks);
		break;
	}

	sequence->tracksRunning = sequence->nTracks;
}

void HBMSEQInit(void)
{
	if (!__init)
	{
		__HBMSEQSequenceList = nullptr;
		__init = TRUE;
	}
}

void HBMSEQQuit(void)
{
	__HBMSEQSequenceList = nullptr;
	__init = FALSE;
}

void HBMSEQRunAudioFrame(void)
{
	HBMSEQSEQUENCE *sequence = __HBMSEQSequenceList;

	if (!__init)
		return;

	for (; sequence; sequence = sequence->next)
	{
		if (sequence->state == 1 || sequence->state == 2)
		{
			for (u32 i = 0; i < sequence->nTracks; i++)
			{
				HBMSEQTRACK *track = &sequence->track[i];

				if (track->state == 1 || track->state == 2)
				{
					u32 ticks = track->ticksPerFrame;

					if (track->delay > ticks)
					{
						track->delay -= ticks;
						continue;
					}

					while (ticks >= track->delay)
					{
						ticks -= track->delay;

						__HBMSEQRunEvent(&sequence->synth, track);

						if (track->state == 0)
							break;

						track->delay = __HBMSEQGetIntTrack(track) << 16;
					}

					track->delay -= ticks;
				}
			}
		}

		if (sequence->end)
		{
			if (sequence->state == 2)
			{
				HBMSEQSetState(sequence, 0);
				HBMSEQSetState(sequence, 2);
			}
			else
			{
				HBMSEQSetState(sequence, 0);
			}
		}
	}
}

void HBMSEQAddSequence(HBMSEQSEQUENCE *sequence, byte_t *midiStream,
                       byte_t *wavetable, byte_t *samples, byte_t *zerobuffer)
{
	HBMSYNInitSynth(&sequence->synth, wavetable, samples, zerobuffer);
	sequence->state = 0;
	__HBMSEQReadHeader(sequence, midiStream);
	__HBMSEQPushSequenceList(sequence);
}

void HBMSEQRemoveSequence(HBMSEQSEQUENCE *sequence)
{
	__HBMSEQRemoveSequenceFromList(sequence);
	HBMSYNQuitSynth(&sequence->synth);
}

void HBMSEQSetState(HBMSEQSEQUENCE *sequence, u32 state)
{
	int i;

	switch (state)
	{
	case 1:
	case 2:
		if (sequence->state == 0)
		{
			BOOL intrStatus = OSDisableInterrupts();

			for (i = 0; i < sequence->nTracks; i++)
			{
				HBMSEQTRACK *track = &sequence->track[i];

				track->current = track->start;
				track->ticksPerFrame = track->defaultTicksPerFrame;
				track->delay = __HBMSEQGetIntTrack(track) << 16;
				track->state = 1;
			}
			sequence->tracksRunning = sequence->nTracks;

			OSRestoreInterrupts(intrStatus);
		}

		sequence->end = 0;
		break;

	case 0:
	case 3:
		for (i = 0; i < HBMSYN_NUM_MIDI_CHANNELS; i++)
		{
			BOOL intrStatus = OSDisableInterrupts();

			byte_t ch[3];
			ch[0] = MIDI_MESSAGE_CHANNEL_MODE_MESSAGE | i;
			ch[1] = MIDI_CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF;
			ch[2] = 0; // v = 0 -> all notes off

			HBMSYNMidiInput(&sequence->synth, ch);

			OSRestoreInterrupts(intrStatus);
		}

		break;
	}

	sequence->state = state;
}

u32 HBMSEQGetState(HBMSEQSEQUENCE *sequence)
{
	return sequence->state;
}

void HBMSEQSetVolume(HBMSEQSEQUENCE *sequence, s32 dB)
{
	HBMSYNSetMasterVolume(&sequence->synth, dB);
}
