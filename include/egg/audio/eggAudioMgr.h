#ifndef EGG_AUDIO_MGR_H
#define EGG_AUDIO_MGR_H

#include <egg/core/eggHeap.h>

namespace EGG {
class IAudioMgr {
public:
	struct Arg {
		Heap* pHeap;              // _00
		u32 _04;                  // _04
		s32 soundThreadPrio;      // _08
		s32 dvdThreadPrio;        // _0C
		u32 soundThreadStackSize; // _10
		u32 dvdThreadStackSize;   // _14
	};

public:
	virtual void initialize(Arg*) { } // _08
	virtual void calc() = 0;          // _0C

protected:
	bool mIsInitialized; // _04
};

class SimpleAudioMgr : public IAudioMgr {
public:
	struct SimpleAudioMgrArg : IAudioMgr::Arg {
		// TODO(kiwi) How is this calculated?
		static const u32 DEFAULT_SOUND_HEAP_SIZE = 0x8CA000;

		u32 streamBlocks;  // _18
		u32 soundHeapSize; // _1C

		SimpleAudioMgrArg();
	};

public:
	SimpleAudioMgr();
	virtual ~SimpleAudioMgr(); // _08

	virtual void initialize(Arg* pArg); // _08
	virtual void calc();                // _0C
};
} // namespace EGG

#endif
