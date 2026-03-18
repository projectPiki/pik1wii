#ifndef REVOSDK_HBM_AX_SOUND_H
#define REVOSDK_HBM_AX_SOUND_H

#include "homebuttonLib/sound/mix.h"
#include <types.h>

namespace homebutton {

	void PlaySeq(int num);
	void InitAxSound(void const *soundData, void *mem, u32 memSize);
	void ShutdownAxSound();
	void AxSoundMain();
	void StopAllSeq();
	void SetVolumeAllSeq(f32 gain);
	void SetSoundMode(HBMMIXSoundMode mode);
} // namespace homebutton

#endif // REVOSDK_HBM_AX_SOUND_H