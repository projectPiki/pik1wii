#include <RevoSDK/OS/OSBootInfo.h>
#include <egg/audio/eggAudioMgr.h>
#include <egg/core/eggController.h>
#include <egg/core/eggDisplay.h>
#include <egg/core/eggExpHeap.h>
#include <egg/core/eggPerf.h>
#include <egg/core/eggSceneManager.h>
#include <egg/core/eggSystem.h>

namespace EGG {

ConfigurationData* BaseSystem::mConfigData;

void ConfigurationData::initMemory()
{
	void* pMEM1ArenaLo = OSGetMEM1ArenaLo();
	void* pMEM1ArenaHi = OSGetMEM1ArenaHi();
	void* pMEM2ArenaLo = OSGetMEM2ArenaLo();
	void* pMEM2ArenaHi = OSGetMEM2ArenaHi();

	void* arena1Start = OSInitAlloc(pMEM1ArenaLo, pMEM1ArenaHi, 2);
	void* arena2Start = OSInitAlloc(pMEM2ArenaLo, pMEM2ArenaHi, 2);

	pMEM1ArenaLo = ROUND_UP_PTR(arena1Start, 32);
	arena1Start  = ROUND_DOWN_PTR(pMEM1ArenaHi, 32);
	pMEM2ArenaLo = ROUND_UP_PTR(arena2Start, 32);
	arena2Start  = ROUND_DOWN_PTR(pMEM2ArenaHi, 32);

	mBootInfo = static_cast<OSBootInfo*>(OSPhysicalToCached(0));

	mSystemHeapStart = pMEM1ArenaLo;
	mMem1Start       = pMEM1ArenaLo;
	mMem1End         = arena1Start;
	mMem2Start       = pMEM2ArenaLo;
	mMem2End         = arena2Start;
	mMemSize         = mBootInfo->memorySize;

	OSSetMEM1ArenaLo(arena1Start);
	OSSetMEM1ArenaHi(arena1Start);
	OSSetMEM2ArenaLo(arena2Start);
	OSSetMEM2ArenaHi(arena2Start);

	Heap::initialize();

	void* root1HeapPtr = mMem1Start;
	u32 root1HeapSize  = (u32)mMem1End - (u32)mMem1Start;

	void* root2HeapPtr = mMem2Start;
	u32 root2HeapSize  = (u32)mMem2End - (u32)mMem2Start;

	u32 debugHeapSize;
	if (root2HeapSize >= 0x4000000) {
		debugHeapSize = 0x4000000;
		root2HeapSize = OSRoundDown32B(root2HeapSize - 0x4000000);
	} else {
		debugHeapSize = 0;
	}
	void* debugHeapPtr = (u8*)root2HeapPtr + root2HeapSize;

	mRootHeapMem1 = ExpHeap::create(root1HeapPtr, root1HeapSize, 0);
	mRootHeapMem1->setName("EGGRootMEM1");

	mRootHeapMem2 = ExpHeap::create(root2HeapPtr, root2HeapSize, 0);
	mRootHeapMem2->setName("EGGRootMEM2");

	if (debugHeapSize > 0) {
		mRootHeapDebug = ExpHeap::create(debugHeapPtr, debugHeapSize, 0);
		mRootHeapDebug->setName("EGGRootDebug");
	} else {
		mRootHeapDebug = nullptr;
	}

	mSystemHeap = ExpHeap::create(mSystemHeapSize, mRootHeapMem1, 0);
	mSystemHeap->setName("EGGSystem");
	mSystemHeap->becomeCurrentHeap();
}

void ConfigurationData::run()
{
	CoreControllerMgr* pControllerMgr = EGG_INSTANCE(CoreControllerMgr);
	GCControllerMgr* pGCControllerMgr = EGG_INSTANCE(GCControllerMgr);

	while (true) {
		getDisplay()->beginFrame();

		onBeginFrame();

		getPerfView()->measureBeginFrame();

		pControllerMgr->beginFrame();
		pGCControllerMgr->calc();

		getDisplay()->beginRender();

		getPerfView()->measureBeginRender();

		getSceneMgr()->draw();

		getPerfView()->draw();

		getPerfView()->measureEndRender();

		getDisplay()->endRender();

		getSceneMgr()->calc();

		pControllerMgr->endFrame();

		if (getAudioMgr()) {
			getAudioMgr()->calc();
		}

		getDisplay()->endFrame();

		onEndFrame();

		getPerfView()->measureEndFrame();
	}
}

} // namespace EGG
