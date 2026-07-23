#ifndef EGG_CORE_SYSTEM_H
#define EGG_CORE_SYSTEM_H

#include <egg/egg_types.h>

#include <RevoSDK/gx.h>
#include <RevoSDK/os.h>
#include <egg/prim/eggBitFlag.h>

struct OSBootInfo;

namespace EGG {

class Display;
class Heap;
class PerformanceView;
class SceneManager;
class SimpleAudioMgr;
class Thread;
class Video;
class XfbManager;

class ConfigurationData {
public:
	explicit ConfigurationData(u32 systemHeapSize)
	    : mSystemHeapSize(systemHeapSize)
	{
	}

	virtual Video* getVideo()              = 0; // _08
	virtual Heap* getSystemHeap()          = 0; // _0C
	virtual Display* getDisplay()          = 0; // _10
	virtual XfbManager* getXfbMgr()        = 0; // _14
	virtual PerformanceView* getPerfView() = 0; // _18
	virtual SceneManager* getSceneMgr()    = 0; // _1C
	virtual IAudioMgr* getAudioMgr()       = 0; // _20
	virtual void onBeginFrame() { }             // _24
	virtual void onEndFrame() { }               // _28
	virtual void initRenderMode() { }           // _2C
	virtual void initMemory();                  // _30
	virtual void run();                         // _34
	virtual void initialize() = 0;              // _38

	Heap* getRootHeapMem1() { return mRootHeapMem1; }
	Heap* getRootHeapMem2() { return mRootHeapMem2; }
	Heap* getRootHeapDebug() { return mRootHeapDebug; }

public:
	void* mMem1Start;       // _04
	void* mMem1End;         // _08
	void* mMem2Start;       // _0C
	void* mMem2End;         // _10
	u32 mMemSize;           // _14
	Heap* mRootHeapMem1;    // _18
	Heap* mRootHeapMem2;    // _1C
	Heap* mRootHeapDebug;   // _20
	Heap* mSystemHeap;      // _24
	Thread* mSystemThread;  // _28
	OSBootInfo* mBootInfo;  // _2C
	void* mSystemHeapStart; // _30
	u32 mSystemHeapSize;    // _34
};
class BaseSystem {
public:
	static ConfigurationData* mConfigData;

	static XfbManager* getXfbMgr() { return mConfigData->getXfbMgr(); }
	static Display* getDisplay() { return mConfigData->getDisplay(); }
	static Video* getVideo() { return mConfigData->getVideo(); }
};

template <class TVideo, class TDisplay, class TXfbManager, class TAudioManager, class TSceneManager, class TPerfView>
class TSystem : public ConfigurationData {
public:
	inline TSystem()
	    : mGraphicsFifoSize(0x80000)
	    , mRenderMode()
	{
	}

	void onBeginFrame() { }
	void onEndFrame() { }

	void initRenderMode() { }
	void initialize() { }

	Video* getVideo() { return static_cast<Video*>(mVideo); }

	Heap* getSystemHeap() { return static_cast<Heap*>(mSystemHeap); }

	Display* getDisplay() { return static_cast<Display*>(mDisplay); }

	XfbManager* getXfbMgr() { return static_cast<XfbManager*>(mXfbMgr); }

	PerformanceView* getPerfView() { return static_cast<PerformanceView*>(mPerfView); }

	SceneManager* getSceneMgr() { return static_cast<SceneManager*>(mSceneMgr); }

	SimpleAudioMgr* getAudioMgr() { return static_cast<SimpleAudioMgr*>(mAudioMgr); }

public:
	u32 mGraphicsFifoSize;        // _38
	GXRenderModeObj* mRenderMode; // _3C
	TAudioManager* mAudioMgr;     // _40
	TVideo* mVideo;               // _44
	TXfbManager* mXfbMgr;         // _48
	TDisplay* mDisplay;           // _4C
	TPerfView* mPerfView;         // _50
	TSceneManager* mSceneMgr;     // _54
};

} // namespace EGG

#endif
