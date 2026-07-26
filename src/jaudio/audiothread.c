#include "jaudio/audiothread.h"
#include "RevoSDK/OS/OSThread.h"
#include "RevoSDK/ai.h"
#include "RevoSDK/dsp.h"
#include "RevoSDK/os.h"
#include "jaudio/aictrl.h"
#include "jaudio/audiocommon.h"
#include "jaudio/cpubuf.h"
#include "jaudio/dspboot.h"
#include "jaudio/dspbuf.h"
#include "jaudio/dspinterface.h"
#include "jaudio/dspproc.h"
#include "jaudio/dsptask.h"
#include "jaudio/dummyprobe.h"
#include "jaudio/dummyrom.h"
#include "jaudio/dvdthread.h"
#include "jaudio/ja_calc.h"
#include "jaudio/playercall.h"
#include "jaudio/rate.h"
#include "jaudio/stackchecker.h"
#include <stddef.h>

// this is purely for comm bullshit to match. pretty sure it's not the comm bug, just wacky.
typedef struct Jac_AudioThread {
	OSThread thread; // _00
	u8 pad[0x10];    // _310
} Jac_AudioThread;

Jac_AudioThread jac_audioThread ATTRIBUTE_ALIGN(32);
u8 jac_audioStack[AUDIO_STACK_SIZE];
OSThread jac_neosThread;
OSThread jac_dvdThread;
u8 jac_dvdStack[AUDIO_STACK_SIZE] ATTRIBUTE_ALIGN(32);

static OSMessageQueue audioproc_mq;
static OSMessage msgbuf[AUDIOPROC_MQ_BUF_COUNT];

static u32 audioproc_mq_init;
static volatile int intcount;

/**
 * @TODO: Documentation
 */
void DspSyncCountClear(int count)
{
	intcount = count;
}

/**
 * @TODO: Documentation
 */
int DspSyncCountCheck()
{
	return intcount;
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000008
 */
void Jac_GetDacRate(void)
{
	// UNUSED FUNCTION
}

/**
 * @TODO: Documentation
 */
static void DspSync()
{
	if (audioproc_mq_init) {
		if (!OSSendMessage(&audioproc_mq, AUDIOPROC_MESSAGE_DSP_SYNC, OS_MESSAGE_NOBLOCK)) {
			OSReport("DSPSync:: SendMiss\n");
			Console_printf("send Miss \n");
		}
	} else {
		DSPReleaseHalt();
	}
}
/**
 * @TODO: Documentation
 */
static void DspSync2(void*)
{
	volatile u32 check;
	u32 mesg;
	u32 stack;

	do {
		check = DSPCheckMailFromDSP();
	} while (check == 0);

	mesg = DSPReadMailFromDSP();
	if (mesg >> 0x10 == 0xf355) {
		stack = 0x10000;
		if (u16(mesg & 0xff00) == stack - 0x100) {
			DspSync();
		} else {
			DspFinishWork(mesg);
		}
	}
}

/**
 * @TODO: Documentation
 */
void StopAudioThread()
{
	if (audioproc_mq_init) {
		if (OSSendMessage(&audioproc_mq, AUDIOPROC_MESSAGE_3, OS_MESSAGE_NOBLOCK) == FALSE) {
			OSCancelThread(&jac_audioThread.thread);
		}
	}
}

/**
 * @TODO: Documentation
 */
static void AudioSync()
{
	static BOOL first = TRUE;

	if (first == FALSE) {
		Probe_Finish(4);
	}

	first = FALSE;
	Probe_Start(4, "UPDATE-DAC");
	if (audioproc_mq_init) {
		OSSendMessage(&audioproc_mq, AUDIOPROC_MESSAGE_UPDATE_DAC, OS_MESSAGE_NOBLOCK);
	}
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 00003C
 */
void NeosSync()
{
	// UNUSED FUNCTION
}

/**
 * @TODO: Documentation
 */
static void* audioproc(void*)
{
	OSReport("\n♪Jac/Thread/Proc:: オーディオスレッドが開始しました\n");
	OSInitFastCast();
	OSInitMessageQueue(&audioproc_mq, msgbuf, AUDIOPROC_MQ_BUF_COUNT);
	audioproc_mq_init = TRUE;
	ResetPlayerCallback();
	Jac_Init();
	Jac_InitSinTable();
	DspbufProcess(DSPBUF_EVENT_INIT);
	CpubufProcess(DSPBUF_EVENT_INIT);
	DspBoot(DspSync2);
	DSP_InitBuffer();
	AISetDSPSampleRate(JAC_AI_SETTING);
	AIRegisterDMACallback(&AudioSync);
	AIStartDMA();
	OSReport("♪Jac/Thread/Proc::オーディオシステムのイニシャルが終了しました\n");
	OSReport("\t\t\t\t\t ここからはループになります\n\n");

	while (TRUE) {
		OSMessage msg;

		OSReceiveMessage(&audioproc_mq, &msg, OS_MESSAGE_BLOCK);
		switch ((int)msg) {
		case (int)AUDIOPROC_MESSAGE_UPDATE_DAC:
		{
			Jac_UpdateDAC();
			break;
		}
		case (int)AUDIOPROC_MESSAGE_DSP_SYNC:
		{
			if (intcount == 0) {
				OSReport("♪Jac/thread/DSPSync:: 余分な割り込みが発生...\n");
				Console_printf("余分\n");
				return;
			}

			intcount--;
			if (intcount == 0) {
				Probe_Finish(7);
				DspFrameEnd();
			} else {
				Probe_Start(2, "SFR_DSP");
				UpdateDSP();
				Probe_Finish(2);
			}

			break;
		}
		case (int)AUDIOPROC_MESSAGE_NEOS_SYNC:
		{
			CpuFrameEnd();
			break;
		}
		case (int)AUDIOPROC_MESSAGE_3:
		{
			OSReport("オーディオスレッドを終了します\n");
			OSExitThread(NULL);
			OSReport("これは侮ｦされたらおかしいです\n");
			break;
		}
		}
	}
}

static BOOL priority_set        = FALSE;
static volatile OSPriority pri  = 0;
static volatile OSPriority pri2 = 0;
static volatile OSPriority pri3 = 0;

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000024
 */
void SetAudioThreadPriority(void)
{
	// UNUSED FUNCTION
}

/**
 * @TODO: Documentation
 */
void StartAudioThread(void* heap, s32 heapSize, u32 aramSize, u32 flags)
{
	if (priority_set == FALSE) {
		OSPriority base_prio = OSGetThreadPriority(OSGetCurrentThread()) - 3;

		pri = base_prio;
		pri3         = base_prio + 1;
		pri2         = base_prio + 2;
		OSReport("♪Jac/Thread/Init:: デフォルトのスレッドプライオリティ値を使用します\n");
	} else {
		OSReport("♪Jac/Thread/Init:: 蘭 済みのスレッドプライオリティ値を使用します\n");
	}

	u32 neos_flag;

	OSReport("\n♪Jac/Thread/Init:: オーディオスレッドを起動します\n");
	Jac_HeapSetup(heap, heapSize);
	Jac_SetAudioARAMSize(aramSize);

	neos_flag = flags & AUDIO_THREAD_FLAG_NEOS;
	Jac_InitARAM(neos_flag);

	OSReport("♪Jac/Thread/Init:: 　スレッドの優先順位は %d  です\n", pri);
	Jac_StackInit(jac_audioStack, 0x400);
	if ((flags & AUDIO_THREAD_FLAG_AUDIO)) {
		// point to top of audioStack
		u8* stack_p = jac_audioStack;
		OSCreateThread(&jac_audioThread.thread, &audioproc, NULL, stack_p + AUDIO_STACK_SIZE, AUDIO_STACK_SIZE, pri, OS_THREAD_ATTR_DETACH);
		OSResumeThread(&jac_audioThread.thread);
		OSReport("♪Jac/Thread/Init:: 　オーディオスレッドを起動終了しました(prio:%d)\n\n", pri);
	}

	Jac_StackInit(jac_dvdStack, 0x400);
	if ((flags & AUDIO_THREAD_FLAG_DVD)) {
		jac_dvdproc_init();
		// point to top of dvdStack
		u8* stack_p = jac_dvdStack;
		OSCreateThread(&jac_dvdThread, &jac_dvdproc, NULL, stack_p + AUDIO_STACK_SIZE, AUDIO_STACK_SIZE, pri3, OS_THREAD_ATTR_DETACH);
		OSResumeThread(&jac_dvdThread);
		OSReport("♪Jac/Thread/Init:: 　DVDスレッドを起動終了しました(prio:%d)\n\n", pri3);
	} else {
		OSReport("♪Jac/Thread/Init:: 　DVDスレッドを起動しませんでした\n\n");
	}
}
