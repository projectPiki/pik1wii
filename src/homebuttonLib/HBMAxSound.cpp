#include "homebuttonLib/HBMAxSound.h"

#include "homebuttonLib/sound/mix.h"
#include "homebuttonLib/sound/seq.h"
#include "homebuttonLib/sound/syn.h"
#include <RevoSDK/ai.h>
#include <RevoSDK/arc.h>
#include <RevoSDK/ax.h>
#include <RevoSDK/os.h>
#include <RevoSDK/hbm.h>
#include <math.h>
#include <types.h>


#define HBM_MEM_SIZE_SOUND	0x18700	/* name known from asserts */

namespace
{
	// AudioSoundThreadProc message values
	enum AudioThreadMsg
	{
		AudioThreadMsg_RunAudioFrame	= 1 << 0,
		AudioThreadMsg_Exit				= 1 << 3
	};

	struct SeqPlayer
	{
		HBMSEQSEQUENCE seq;		// _00
		bool activeFlag;		// _2E1C
		SeqPlayer *next;		// _2E20
		SeqPlayer *prev;		// _2E24
		int seqNum;				// _2e28
	};

	struct SeqPlayerList
	{
		SeqPlayer *head;	// _00
		SeqPlayer *tail;	// _04
	};
} // namespace

struct HMBAxSoundWork // NOTE the misspelled tag name
{
	SeqPlayer playersDefault[4];				// _00
	SeqPlayer playersFocus[3];					// _B8B0
	SeqPlayerList playerListDefault;			// _14334
	SeqPlayerList playerListFocus;				// _1433C
	HBMSoundCallback *userSoundCallback;		// _14344
	AXOutCallback oldAxRegisterCallback;		// _14348
	ARCHandle soundArcHandle;					// _1434C
	OSThread mThread;							// _14368
	OSMessageQueue mMsgQueue;					// _14680
	OSMessage mMessageArray[4];					// _146A0
	byte8_t *mpThreadStack;						// _146B0
	void *wt;									// _146B4
	void *pcm;									// _146B8
}; 

namespace {


static const char WT_FILENAME[] = "wt\\HomeButtonSe.wt";
static const char PCM_FILENAME[] = "wt\\HomeButtonSe.pcm";

HMBAxSoundWork *sWork;


void *GetMidiDataFromArc(int num)
{
	static const char *SOUND_FILENAME[] =
	{
		"midi\\00_home_button.mid",
		"midi\\01_return_app.mid",
		"midi\\02_goto_menu.mid",
		"midi\\03_reset_app.mid",
		"midi\\04_focus.mid",
		"midi\\05_select.mid",
		"midi\\06_cancel.mid",
		"midi\\07_open_controller.mid",
		"midi\\08_close_controller.mid",
		"midi\\09_volume_plus.mid",
		"midi\\10_volume_minus.mid",
		"midi\\11_volume_plus_limit.mid",
		"midi\\12_volume_minus_limit.mid",
		"midi\\13_nothing_done.mid",
		"midi\\14_vibe_on.mid",
		"midi\\15_vibe_off.mid",
		"midi\\16_start_connect_window.mid",
		"midi\\17_connected1.mid",
		"midi\\18_connected2.mid",
		"midi\\19_connected3.mid",
		"midi\\20_connected4.mid",
		"midi\\21_end_connect_window.mid",
		"midi\\22_manual_open.mid",
		"midi\\23_manual_focus.mid",
		"midi\\24_manual_select.mid",
		"midi\\25_manual_scroll.mid",
		"midi\\26_manual_cancel.mid",
		"midi\\27_manual_return_app.mid"
	};

	ARCFileInfo info;
	BOOL result = ARCOpen(&sWork->soundArcHandle, SOUND_FILENAME[num], &info);
	if (!result)
		return nullptr;

	return ARCGetStartAddrInMem(&info);
}

SeqPlayerList *GetUsePlayerListFromSeqNum(int num)
{
	switch (num)
	{
	case HBM_SOUND_FOCUS:
	case HBM_SOUND_MANUAL_FOCUS:
	case HBM_SOUND_MANUAL_SCROLL:
		return &sWork->playerListFocus;

	default:
		return &sWork->playerListDefault;
	}
}

void StopSeq(SeqPlayer *player)
{
	SeqPlayerList *list = GetUsePlayerListFromSeqNum(player->seqNum);

	HBMSEQSetState(&player->seq, HBM_SEQ_STATE_0);
	HBMSEQRemoveSequence(&player->seq);

	player->activeFlag = false;

	if (!player->prev)
		list->head = player->next;
	else
		player->prev->next = player->next;

	if (!player->next)
		list->tail = player->prev;
	else
		player->next->prev = player->prev;

	player->next = nullptr;
	player->prev = nullptr;
}

void UpdateSeqPlayerList(SeqPlayerList *list)
{
	SeqPlayer *current = list->head;
	for (SeqPlayer *next; current; current = next)
	{
		next = current->next;

		if (current->activeFlag && HBMSEQGetState(&current->seq) == HBM_SEQ_STATE_0)
		{
			StopSeq(current);
		}
	}
}

SeqPlayer *GetFreePlayer(int num)
{
	SeqPlayer *playersArray;
	int arraySize;

	switch (num)
	{
	case HBM_SOUND_FOCUS:
	case HBM_SOUND_MANUAL_FOCUS:
	case HBM_SOUND_MANUAL_SCROLL:
		playersArray = sWork->playersFocus;
		arraySize = ARRAY_SIZE(sWork->playersFocus);
		break;

	default:
		playersArray = sWork->playersDefault;
		arraySize = ARRAY_SIZE(sWork->playersDefault);
		break;
	}

	SeqPlayer *player = nullptr;

	for (int i = 0; i < arraySize; i++)
	{
		if (!playersArray[i].activeFlag)
		{
			player = &playersArray[i];
			player->activeFlag = true;
			break;
		}
	}

	if (!player)
	{
		SeqPlayerList *list = GetUsePlayerListFromSeqNum(num);

		player = list->head;
		StopSeq(list->head);
		player->activeFlag = true;
	}

	return player;
}

void AudioFrameCallback()
{
	if (!sWork)
		return;

	BOOL result = OSSendMessageAny(&sWork->mMsgQueue, AudioThreadMsg_RunAudioFrame, OS_MSG_NO_FLAGS);

	if (sWork->oldAxRegisterCallback)
		(*sWork->oldAxRegisterCallback)();
}

void *AudioSoundThreadProc(void *)
{
	OSMessage message = OSMessage();

	while (true)
	{
		OSReceiveMessage(&sWork->mMsgQueue, &message, OS_MSG_PERSISTENT);

		if (reinterpret_cast<u32>(message) == AudioThreadMsg_RunAudioFrame)
		{
			HBMSEQRunAudioFrame();
			HBMSYNRunAudioFrame();
			HBMMIXUpdateSettings();

			if (sWork)
			{
				UpdateSeqPlayerList(&sWork->playerListDefault);
				UpdateSeqPlayerList(&sWork->playerListFocus);
			}
		}
		else if (reinterpret_cast<u32>(message) == AudioThreadMsg_Exit)
		{
			break;
		}
	}

	return nullptr;
}

} // namespace

namespace homebutton {

void PlaySeq(int num)
{
	void *midi = GetMidiDataFromArc(num);
	if (!midi)
		return;

	SeqPlayer *player = GetFreePlayer(num);
	HBMSEQAddSequence(&player->seq, static_cast<byte_t *>(midi),
	                  static_cast<byte_t *>(sWork->wt),
	                  static_cast<byte_t *>(sWork->pcm), nullptr);
	HBMSEQSetState(&player->seq, HBM_SEQ_STATE_1);
	player->seqNum = num;

	SeqPlayerList *list = GetUsePlayerListFromSeqNum(num);

	if (!list->tail)
	{
		list->head = player;
		list->tail = player;

		player->next = nullptr;
		player->prev = nullptr;
	}
	else
	{
		list->tail->next = player;

		player->prev = list->tail;
		player->next = nullptr;

		list->tail = player;
	}
}

void InitAxSound(void const *soundData, void *mem, u32 memSize)
{
	if (memSize < HBM_MEM_SIZE_SOUND)
		return;

	if (!AICheckInit())
	{
		AIInit(nullptr);
		AXInit();
	}

	HBMMIXInit();
	HBMSYNInit();
	HBMSEQInit();

	HMBAxSoundWork *work = static_cast<HMBAxSoundWork *>(mem);

	for (int i = 0; i < (int)ARRAY_SIZE(work->playersDefault); ++i)
	{
		work->playersDefault[i].activeFlag = false;
		work->playersDefault[i].next = nullptr;
		work->playersDefault[i].prev = nullptr;
	}

	for (int i = 0; i < (int)ARRAY_SIZE(work->playersFocus); ++i)
	{
		work->playersFocus[i].activeFlag = false;
		work->playersFocus[i].next = nullptr;
		work->playersFocus[i].prev = nullptr;
	}

	work->wt = nullptr;
	work->pcm = nullptr;
	work->mpThreadStack = reinterpret_cast<u64 *>(
		reinterpret_cast<u32>(mem) + (HBM_MEM_SIZE_SOUND - 0x4000));
	work->playerListDefault.head = nullptr;
	work->playerListDefault.tail = nullptr;
	work->playerListFocus.head = nullptr;
	work->playerListFocus.tail = nullptr;

	BOOL result = ARCInitHandle(const_cast<void *>(soundData), &work->soundArcHandle);
	if (!result)
		return;

	ARCFileInfo wtInfo;
	result = ARCOpen(&work->soundArcHandle, WT_FILENAME, &wtInfo);
	if (!result)
		return;

	work->wt = ARCGetStartAddrInMem(&wtInfo);

	ARCFileInfo pcmInfo;
	result = ARCOpen(&work->soundArcHandle, PCM_FILENAME, &pcmInfo);
	if (!result)
		return;

	work->pcm = ARCGetStartAddrInMem(&pcmInfo);

	OSInitMessageQueue(&work->mMsgQueue, work->mMessageArray,
	                   ARRAY_SIZE(work->mMessageArray));

	result = OSCreateThread(&work->mThread, AudioSoundThreadProc, nullptr,
	                        reinterpret_cast<byte_t *>(work->mpThreadStack)
	                            + memSize - (HBM_MEM_SIZE_SOUND - 0x4000),
	                        memSize - (HBM_MEM_SIZE_SOUND - 0x4000), 4,
	                        OS_THREAD_ATTR_NO_FLAGS);
	if (!result)
		return;

	sWork = work;
	OSResumeThread(&work->mThread);

	work->oldAxRegisterCallback = AXRegisterCallback(&AudioFrameCallback);

}

void ShutdownAxSound()
{
	if (!sWork)
		return;

	StopAllSeq();
	AXRegisterCallback(sWork->oldAxRegisterCallback);

  	BOOL result = OSJamMessageAny(&sWork->mMsgQueue, AudioThreadMsg_Exit, OS_MSG_PERSISTENT);

	OSJoinThread(&sWork->mThread, nullptr);
	HBMSEQQuit();
	HBMSYNQuit();
	HBMMIXQuit();
	sWork = nullptr;
}

void AxSoundMain()
{
	if (!sWork)
		return;
}

void StopAllSeq()
{
	if (!sWork)
		return;

	for (int i = 0; i < (int)ARRAY_SIZE(sWork->playersDefault); ++i)
	{
		SeqPlayer &player = sWork->playersDefault[i];

		if (player.activeFlag)
			StopSeq(&player);
	}

	for (int i = 0; i < (int)ARRAY_SIZE(sWork->playersFocus); ++i)
	{
		SeqPlayer &player = sWork->playersFocus[i];

		if (player.activeFlag)
			StopSeq(&player);
	}
}

void SetVolumeAllSeq(f32 gain)
{
	if (!sWork)
		return;

	s32 db = gain <= 0.0f ? -904
	                      : static_cast<s32>(log10f(gain) * 20.0f * 10.0f);

	if (db > 60)
		db = 60;

	if (db < -904)
		db = -904;

	for (int i = 0; i < (int)ARRAY_SIZE(sWork->playersDefault); ++i)
	{
		SeqPlayer &player = sWork->playersDefault[i];

		if (player.activeFlag)
			HBMSEQSetVolume(&player.seq, db);
	}

	for (int i = 0; i < (int)ARRAY_SIZE(sWork->playersFocus); ++i)
	{
		SeqPlayer &player = sWork->playersFocus[i];

		if (player.activeFlag)
			HBMSEQSetVolume(&player.seq, db);
	}
}

void SetSoundMode(HBMMIXSoundMode mode)
{
	HBMMIXSetSoundMode(mode);
}

} // namespace homebutton