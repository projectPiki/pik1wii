#include <RevoSDK/hbm.h>
#include "homebuttonLib/HBMCommon.h"
#include "homebuttonLib/HBMAxSound.h"
#include "homebuttonLib/HBMBase.h"
#include "homebuttonLib/nw4hbm/lyt/layout.h"
#include <RevoSDK/mem.h>
#include <RevoSDK/os.h>
#include <types.h>

enum HBMAllocatorType
{
	HBM_ALLOCATOR_APPLI/*cation*/,
	HBM_ALLOCATOR_LOCAL,
	HBM_ALLOCATOR_NW4R
};

char const *__HBMVersion = "<< RVL_SDK - HBM \trelease build: Feb  7 2008 15:55:00 (0x4199_60726) >>";

static MEMAllocator sAllocator;
MEMAllocator *spAllocator = &sAllocator;

void *HBMAllocMem(u32 size)
{
	void *addr = MEMAllocFromAllocator(spAllocator, size);

	return addr;
}

void HBMFreeMem(void *mem)
{
	MEMFreeToAllocator(spAllocator, mem);
}

static HBMAllocatorType getAllocatorType(HBMDataInfo const *pHBInfo)
{
	if (pHBInfo->pAllocator)
		return HBM_ALLOCATOR_APPLI;
	else if (pHBInfo->mem)
		return HBM_ALLOCATOR_LOCAL;
	else
		return HBM_ALLOCATOR_NW4R;
}

void HBMCreate(HBMDataInfo const *pHBInfo)
{
	MEMiHeapHead *hExpHeap;

	if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL)
	{
		hExpHeap = MEMCreateExpHeap(pHBInfo->mem, pHBInfo->memSize);
		MEMInitAllocatorForExpHeap(&sAllocator, hExpHeap, 32);
		spAllocator = &sAllocator;
	}

	switch (getAllocatorType(pHBInfo))
	{
	case HBM_ALLOCATOR_APPLI:
		nw4hbm::lyt::Layout::SetAllocator(pHBInfo->pAllocator);
		spAllocator = pHBInfo->pAllocator;
		break;

	case HBM_ALLOCATOR_LOCAL:
		nw4hbm::lyt::Layout::SetAllocator(spAllocator);
		break;

	case HBM_ALLOCATOR_NW4R:
		spAllocator = nw4hbm::lyt::Layout::GetAllocator();
		break;
	}

	homebutton::HomeButton::createInstance(pHBInfo);
	homebutton::HomeButton::getInstance()->create();
}

void HBMDelete(void)
{
	HBMDataInfo const *pHBInfo =
		homebutton::HomeButton::getInstance()->getHBMDataInfo();

	homebutton::HomeButton::deleteInstance();

	if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL)
		MEMDestroyExpHeap(spAllocator->heap);
}

void HBMInit(void)
{
	OSRegisterVersion(__HBMVersion);
	homebutton::HomeButton::getInstance()->init();
}

HBMSelectBtnNum HBMCalc(HBMControllerData const *pController)
{
	homebutton::HomeButton::getInstance()->calc(pController);
	return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMDraw(void)
{
	homebutton::HomeButton::getInstance()->draw();
}

HBMSelectBtnNum HBMGetSelectBtnNum(void)
{
	return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMSetAdjustFlag(BOOL flag)
{
	homebutton::HomeButton::getInstance()->setAdjustFlag(flag);
}

void HBMStartBlackOut(void)
{
	homebutton::HomeButton::getInstance()->startBlackOut();
}

void HBMCreateSound(void *soundData, void *memBuf, u32 memSize)
{
	HBMDataInfo const *pHBInfo =
		homebutton::HomeButton::getInstance()->getHBMDataInfo();

	homebutton::InitAxSound(soundData, memBuf, memSize);
}

void HBMDeleteSound(void)
{
	homebutton::ShutdownAxSound();
}

void HBMUpdateSound(void)
{
	homebutton::HomeButton::getInstance()->update_sound();
}
