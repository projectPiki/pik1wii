#include "homebuttonLib/sound/syn.h"

#include <RevoSDK/AX.h>
#include <types.h>


static u32 __HBMSYNGetNibbleAddress(const u32 count)
{
	const u32 samples = count;
	u32 frames = samples / 14;
	u32 samplesLeft = samples % 14;

	return 2 + frames * 16 + samplesLeft;
}

static void __HBMSYNSetupAdpcm(HBMSYNVOICE *voice)
{
	AXVPB *axvpb = voice->axvpb;

	if (voice->region->loopStart + voice->region->loopLength != 0)
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;
		byte4_t *adpcm;
		u16 *adpcmloop;

		adpcm = (byte4_t *)voice->adpcm;
		voice->type = 1;

		sampleStart = voice->synth->samplesBaseNibble + voice->sample->offset;
		sampleLoop =
			sampleStart + __HBMSYNGetNibbleAddress(voice->region->loopStart);
		sampleEnd = sampleStart
		          + __HBMSYNGetNibbleAddress(voice->region->loopStart
		                                     + voice->region->loopLength - 1);

		sampleStart += sizeof(u16);
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x00010000; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[1][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[2][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[3][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[4][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[5][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[6][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[7][{0,1}]          */ *p = *adpcm; p++; adpcm++;

		/* adpcm.{gain,pred_scale}    */ *p = *adpcm; p++; adpcm++;
		/* adpcm.yn{1,2}              */ *p = *adpcm;
		// clang-format on

		adpcmloop = (u16 *)++adpcm;

		// clang-format off
		axvpb->pb.adpcmLoop.loop_pred_scale = *adpcmloop; adpcmloop++;
		axvpb->pb.adpcmLoop.loop_yn1 = *adpcmloop; adpcmloop++;
		axvpb->pb.adpcmLoop.loop_yn2 = *adpcmloop;
		// clang-format on

		axvpb->sync &=
			~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR
		      | AX_PBSYNC_END_ADDR
		      | AX_PBSYNC_CURR_ADDR);

		axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM
		             | AX_PBSYNC_ADPCM_LOOP;
	}
	else
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;
		byte4_t *adpcm;

		adpcm = (byte4_t *)voice->adpcm;
		voice->type = 0;

		sampleStart = voice->synth->samplesBaseNibble + voice->sample->offset;
		sampleLoop = voice->synth->samplesBaseNibble + voice->sample->offset;
		sampleEnd =
			sampleStart + __HBMSYNGetNibbleAddress(voice->sample->length - 1);

		sampleStart += sizeof(u16);
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x00000000; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[1][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[2][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[3][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[4][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[5][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[6][{0,1}]          */ *p = *adpcm; p++; adpcm++;
		/* adpcm.a[7][{0,1}]          */ *p = *adpcm; p++; adpcm++;

		/* adpcm.{gain,pred_scale}    */ *p = *adpcm; p++; adpcm++;
		/* adpcm.yn{1,2}              */ *p = *adpcm;
		// clang-format on

		axvpb->sync &=
			~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR
		      | AX_PBSYNC_END_ADDR
		      | AX_PBSYNC_CURR_ADDR);

		axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
	}
}

static void __HBMSYNSetupPcm16(HBMSYNVOICE *voice)
{
	AXVPB *axvpb = voice->axvpb;

	if (voice->region->loopStart + voice->region->loopLength != 0)
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;

		voice->type = 1;

		sampleStart = voice->synth->samplesBaseWord + voice->sample->offset;
		sampleLoop = sampleStart + voice->region->loopStart;
		sampleEnd = sampleLoop + voice->region->loopLength - 1;
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x0001000a; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[1][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[2][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[3][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[4][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[5][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[6][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[7][{0,1}]          */ *p = 0; p++;

		/* adpcm.{gain,pred_scale}    */ *p = 0x08000000; p++;
		/* adpcm.yn{1,2}              */ *p = 0x00000000;
		// clang-format on
	}
	else
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;

		voice->type = 0;

		sampleStart = voice->synth->samplesBaseWord + voice->sample->offset;
		sampleLoop = voice->synth->samplesBaseWord + voice->sample->offset;
		sampleEnd = sampleStart + voice->sample->length - 1;
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x0000000a; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[1][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[2][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[3][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[4][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[5][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[6][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[7][{0,1}]          */ *p = 0; p++;

		/* adpcm.{gain,pred_scale}    */ *p = 0x08000000; p++;
		/* adpcm.yn{1,2}              */ *p = 0x00000000;
		// clang-format on
	}

	axvpb->sync &= ~(
		AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR
		| AX_PBSYNC_END_ADDR | AX_PBSYNC_CURR_ADDR);

	axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
}

static void __HBMSYNSetupPcm8(HBMSYNVOICE *voice)
{
	AXVPB *axvpb = voice->axvpb;

	if (voice->region->loopStart + voice->region->loopLength != 0)
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;

		voice->type = 1;

		sampleStart = voice->synth->samplesBaseByte + voice->sample->offset;
		sampleLoop = sampleStart + voice->region->loopStart;
		sampleEnd = sampleLoop + voice->region->loopLength - 1;
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x00010019; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[1][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[2][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[3][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[4][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[5][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[6][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[7][{0,1}]          */ *p = 0; p++;

		/* adpcm.{gain,pred_scale}    */ *p = 0x01000000; p++;
		/* adpcm.yn{1,2}              */ *p = 0x00000000;
		// clang-format on
	}
	else
	{
		u32 sampleStart;
		u32 sampleLoop;
		u32 sampleEnd;
		byte4_t *p;

		voice->type = 0;

		sampleStart = voice->synth->samplesBaseByte + voice->sample->offset;
		sampleLoop = voice->synth->samplesBaseByte + voice->sample->offset;
		sampleEnd = sampleStart + voice->sample->length - 1;
		p = (byte4_t *)&axvpb->pb.addr;

		// clang-format off
		/* addr.{loopFlag,format}     */ *p = 0x00000019; p++;
		/* addr.loopAddress{Hi,Lo}    */ *p = sampleLoop; p++;
		/* addr.endAddress{Hi,Lo}     */ *p = sampleEnd; p++;
		/* addr.currentAddress{Hi,Lo} */ *p = sampleStart; p++;

		/* adpcm.a[0][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[1][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[2][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[3][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[4][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[5][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[6][{0,1}]          */ *p = 0; p++;
		/* adpcm.a[7][{0,1}]          */ *p = 0; p++;

		/* adpcm.{gain,pred_scale}    */ *p = 0x01000000; p++;
		/* adpcm.yn{1,2}              */ *p = 0x00000000;
		// clang-format on
	}

	axvpb->sync &= ~(
		AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR
		| AX_PBSYNC_END_ADDR | AX_PBSYNC_CURR_ADDR);

	axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
}

void __HBMSYNSetupSample(HBMSYNVOICE *voice)
{
	switch (voice->sample->format)
	{
	case 0:
		__HBMSYNSetupAdpcm(voice);
		break;

	case 1:
		__HBMSYNSetupPcm16(voice);
		break;

	case 2:
		__HBMSYNSetupPcm8(voice);
		break;
	}
}
