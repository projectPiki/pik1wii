#include "homebuttonLib/HBMGUIManager.h"
#include "homebuttonLib/nw4hbm/lyt/bounding.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/layout.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/lyt/picture.h"
#include "homebuttonLib/nw4hbm/lyt/window.h"
#include "homebuttonLib/nw4hbm/math/types.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include "homebuttonLib/nw4hbm/ut/list.h"
#include "homebuttonLib/nw4hbm/ut/Rect.h"
#include "homebuttonLib/nw4hbm/ut/RuntimeTypeInfo.h"
#include <types.h>
#include <new.h>

namespace homebutton { 
namespace gui {

u32 PaneManager::suIDCounter;

static void drawLine_(f32 x0, f32 y0, f32 x1, f32 y1, f32 z, u8 uWidth,
                      GXColor &rColor)
{
	static f32 const cubeScale = 1.0f;

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGBA, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	GXSetCullMode(GX_CULL_NONE);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL,
	              GX_DF_NONE, GX_AF_NONE);

	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_NOOP);

	Mtx modelMtx;
	PSMTXTrans(modelMtx, 0.0f, 0.0f, 0.0f);
	GXLoadPosMtxImm(modelMtx, 0);

	GXSetLineWidth(uWidth, GX_TO_ZERO);

	GXBegin(GX_LINES, GX_VTXFMT0, 2);
		GXPosition3f32(x0, y0, z);
		GXColor1u32(*reinterpret_cast<u32 *>(&rColor));

		GXPosition3f32(x1, y1, z);
		GXColor1u32(*reinterpret_cast<u32 *>(&rColor));
	GXEnd();
}

bool Component::update(int i, f32 x, f32 y, u32, u32, u32, void *pData)
{
	bool bTouched = false;

	if (isVisible())
	{
		if (contain(x, y))
		{
			if (isPointed(i))
			{
				onMove(x, y);
				mpManager->onEvent(getID(), EventHandler::Event3, pData);
			}
			else
			{
				setPointed(i, true);
				onPoint();
				mpManager->onEvent(getID(), EventHandler::PointEvent, pData);
			}

			bTouched = true;
		}
		else
		{
			if (isPointed(i))
			{
				setPointed(i, false);
				offPoint();
				mpManager->onEvent(getID(), EventHandler::LeftEvent, pData);
			}
		}
	}

	return bTouched;
}

Manager::~Manager()
{
	void *p;

	for (p = nw4hbm::ut::List_GetFirst(&mIDToComponent); p;
	     p = nw4hbm::ut::List_GetFirst(&mIDToComponent))
	{
		nw4hbm::ut::List_Remove(&mIDToComponent, p);

		if (mpAllocator)
			MEMFreeToAllocator(mpAllocator, p);
		else
			delete static_cast<IDToComponent *>(p);
	}
}

void Manager::init()
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		IDToComponent const *p = static_cast<IDToComponent const *>(
			nw4hbm::ut::List_GetNthConst(&mIDToComponent, i));

		p->mpComponent->init();
	}
}

void Manager::addComponent(Component *pComponent)
{
	u32 uID = pComponent->getID();
	pComponent->setManager(this);

	if (mpAllocator)
	{
		void *p = MEMAllocFromAllocator(mpAllocator, sizeof(IDToComponent));
		nw4hbm::ut::List_Append(&mIDToComponent,
		                        new (p) IDToComponent(uID, pComponent));
	}
	else
	{
		nw4hbm::ut::List_Append(&mIDToComponent,
		                        new IDToComponent(uID, pComponent));
	}
}

void Manager::delComponent(Component *pComponent)
{
	IDToComponent *p;

	for (p = static_cast<IDToComponent *>(
			 nw4hbm::ut::List_GetNext(&mIDToComponent, nullptr));
	     p; p = static_cast<IDToComponent *>(
				nw4hbm::ut::List_GetNext(&mIDToComponent, p)))
	{
		if (p->mpComponent == pComponent)
			break;
	}

	nw4hbm::ut::List_Remove(&mIDToComponent, p);

	if (mpAllocator)
		MEMFreeToAllocator(mpAllocator, p);
	else
		delete p;
}

Component *Manager::getComponent(u32 uID)
{
	IDToComponent const *p = static_cast<IDToComponent const *>(
		nw4hbm::ut::List_GetNthConst(&mIDToComponent, uID));

	return p->mpComponent;
}

bool Manager::update(int i, f32 x, f32 y, u32 uTrigFlag, u32 uHoldFlag,
                     u32 uReleaseFlag, void *pData)
{
	bool bTouched = false;
	Component *pLastContainedComponent = nullptr;

	for (u32 n = 0; n < nw4hbm::ut::List_GetSize(&mIDToComponent); ++n)
	{
		IDToComponent const *p = static_cast<IDToComponent const *>(
			nw4hbm::ut::List_GetNthConst(&mIDToComponent, n));

		if (p->mpComponent->update(i, x, y, uTrigFlag, uHoldFlag, uReleaseFlag,
		                           pData))
		{
			if (p->mpComponent->isTriggerTarger())
				pLastContainedComponent = p->mpComponent;

			bTouched = true;
		}
	}

	if (pLastContainedComponent)
	{
		if (uTrigFlag)
		{
			Vec pos;
			pLastContainedComponent->onTrig(uTrigFlag, pos);

			onEvent(pLastContainedComponent->getID(), EventHandler::TrigEvent,
			        pData);
		}

		if (uReleaseFlag)
		{
			Vec pos;
			pLastContainedComponent->onTrig(uReleaseFlag, pos);

			onEvent(pLastContainedComponent->getID(),
			        EventHandler::ReleaseEvent, pData);
		}
	}

	return bTouched;
}

void Manager::calc()
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		IDToComponent const *p = static_cast<IDToComponent const *>(
			nw4hbm::ut::List_GetNthConst(&mIDToComponent, i));

		p->mpComponent->calc();
	}
}

void Manager::draw()
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		IDToComponent const *p = static_cast<IDToComponent const *>(
			nw4hbm::ut::List_GetNthConst(&mIDToComponent, i));

		p->mpComponent->draw();
	}
}

void Manager::setAllComponentTriggerTarget(bool b)
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		IDToComponent const *p = static_cast<IDToComponent const *>(
			nw4hbm::ut::List_GetNthConst(&mIDToComponent, i));

		p->mpComponent->setTriggerTarget(b);
	}
}

PaneManager::~PaneManager()
{
	PaneToComponent *pPaneToComponent;

	for (pPaneToComponent = static_cast<PaneToComponent *>(
			 nw4hbm::ut::List_GetFirst(&mPaneToComponent));
	     pPaneToComponent; pPaneToComponent = static_cast<PaneToComponent *>(
							   nw4hbm::ut::List_GetFirst(&mPaneToComponent)))
	{
		nw4hbm::ut::List_Remove(&mPaneToComponent, pPaneToComponent);

		if (mpAllocator)
		{
			MEMFreeToAllocator(mpAllocator, pPaneToComponent->mpComponent);
			MEMFreeToAllocator(mpAllocator, pPaneToComponent);
		}
		else
		{
			delete pPaneToComponent->mpComponent;
			delete pPaneToComponent;
		}
	}
}

void PaneManager::createLayoutScene(nw4hbm::lyt::Layout const &rLayout)
{
	suIDCounter = 0;

	nw4hbm::lyt::Pane *pRootPane = rLayout.GetRootPane();

	walkInChildren(pRootPane->GetChildList());
}

void PaneManager::addLayoutScene(nw4hbm::lyt::Layout const &rLayout)
{
	nw4hbm::lyt::Pane *pRootPane = rLayout.GetRootPane();

	walkInChildren(pRootPane->GetChildList());
}

void PaneManager::walkInChildren(nw4hbm::lyt::Pane::LinkList &rPaneList)
{
	NW4HBM_RANGE_FOR(it, rPaneList)
	{
		PaneComponent *pPaneComponent = nullptr;
		PaneToComponent *pPaneToComponent = nullptr;

		if (mpAllocator)
		{
			void *p1 =
				MEMAllocFromAllocator(mpAllocator, sizeof *pPaneComponent);
			void *p2 =
				MEMAllocFromAllocator(mpAllocator, sizeof *pPaneToComponent);

			pPaneComponent = new (p1) PaneComponent(suIDCounter);
			pPaneToComponent = new (p2) PaneToComponent(&(*it), pPaneComponent);
		}
		else
		{
			pPaneComponent = new PaneComponent(suIDCounter);
			pPaneToComponent = new PaneToComponent(&(*it), pPaneComponent);
		}

		nw4hbm::ut::List_Append(&mPaneToComponent, pPaneToComponent);
		++suIDCounter;

		pPaneComponent->setPane(&(*it));

		if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Picture *>(&(*it)))
			pPaneComponent->setTriggerTarget(true);

		if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Window *>(&(*it)))
			pPaneComponent->setTriggerTarget(true);

		addComponent(pPaneComponent);
		walkInChildren(it->GetChildList());
	}
}

void PaneManager::delLayoutScene(nw4hbm::lyt::Layout const &rLayout)
{
	nw4hbm::lyt::Pane *pRootPane = rLayout.GetRootPane();

	walkInChildrenDel(pRootPane->GetChildList());
}

void PaneManager::walkInChildrenDel(nw4hbm::lyt::Pane::LinkList &rPaneList)
{
	NW4HBM_RANGE_FOR(it, rPaneList)
	{
		PaneToComponent *pPaneToComponent;

		for (pPaneToComponent = static_cast<PaneToComponent *>(
				 nw4hbm::ut::List_GetNext(&mPaneToComponent, nullptr));
		     pPaneToComponent;
		     pPaneToComponent = static_cast<PaneToComponent *>(
				 nw4hbm::ut::List_GetNext(&mPaneToComponent, pPaneToComponent)))
		{
			if (pPaneToComponent->mpPane == &(*it))
				break;
		}

		delComponent(pPaneToComponent->mpComponent);
		nw4hbm::ut::List_Remove(&mPaneToComponent, pPaneToComponent);
		--suIDCounter;

		if (mpAllocator)
		{
			MEMFreeToAllocator(mpAllocator, pPaneToComponent->mpComponent);
			MEMFreeToAllocator(mpAllocator, pPaneToComponent);
		}
		else
		{
			delete pPaneToComponent->mpComponent;
			delete pPaneToComponent;
		}

		walkInChildrenDel(it->GetChildList());
	}
}

PaneComponent *PaneManager::getPaneComponentByPane(nw4hbm::lyt::Pane *pPane)
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		PaneToComponent *p = static_cast<PaneToComponent *>(
			nw4hbm::ut::List_GetNth(&mPaneToComponent, i));

		if (p->mpPane == pPane)
			return p->mpComponent;
	}

	return nullptr;
}

#pragma push


# pragma ppc_iro_level 0

void PaneManager::setAllBoundingBoxComponentTriggerTarget(bool b)
{
	for (u32 i = 0; i < nw4hbm::ut::List_GetSize(&mIDToComponent); ++i)
	{
		PaneToComponent *p = static_cast<PaneToComponent *>(
			nw4hbm::ut::List_GetNth(&mPaneToComponent, i));

		if (nw4hbm::ut::DynamicCast<nw4hbm::lyt::Bounding *>(p->mpPane))
			p->mpComponent->setTriggerTarget(b);
	}
}

#pragma pop

bool PaneComponent::contain(f32 x_, f32 y_)
{
	if (!mpManager)
		return false;

	// goes into PaneManager vtable
	nw4hbm::lyt::DrawInfo const *pDrawInfo =
		static_cast<PaneManager *>(mpManager)->getDrawInfo();

	if (!pDrawInfo)
		return false;

	nw4hbm::math::MTX34 invGlbMtx;
	PSMTXInverse(mpPane->GetGlobalMtx(), invGlbMtx);

	nw4hbm::math::VEC3 lclPos;
	PSMTXMultVec(invGlbMtx, nw4hbm::math::VEC3(x_, y_, 0.0f), lclPos);

	nw4hbm::ut::Rect rect = mpPane->GetPaneRect(*pDrawInfo);

	if (rect.left <= lclPos.x && lclPos.x <= rect.right
	    && rect.bottom <= lclPos.y && lclPos.y <= rect.top)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PaneComponent::draw()
{
	nw4hbm::lyt::DrawInfo const *pDrawInfo =
		static_cast<PaneManager *>(mpManager)->getDrawInfo();

	if (!pDrawInfo)
		return;

	nw4hbm::math::VEC3 const &translate = mpPane->GetTranslate();

	nw4hbm::lyt::Size size = mpPane->GetSize();

	nw4hbm::math::MTX34 const &gmtx = mpPane->GetGlobalMtx();

	f32 x = gmtx._03;
	f32 y = gmtx._13;

	GXColor color = {0xff, 0x00, 0x00, 0xff}; // red

	if (mabPointed[0])
	{
		color.r = 0x00;
		color.b = 0xff; // blue!
	}

	// start at top left, go clockwise
	drawLine_(x - size.width / 2.0f, y - size.height / 2.0f,
	          x + size.width / 2.0f, y - size.height / 2.0f, 0.0f, 8, color);
	drawLine_(x + size.width / 2.0f, y - size.height / 2.0f,
	          x + size.width / 2.0f, y + size.height / 2.0f, 0.0f, 8, color);
	drawLine_(x + size.width / 2.0f, y + size.height / 2.0f,
	          x - size.width / 2.0f, y + size.height / 2.0f, 0.0f, 8, color);
	drawLine_(x - size.width / 2.0f, y + size.height / 2.0f,
	          x - size.width / 2.0f, y - size.height / 2.0f, 0.0f, 8, color);
}

#pragma push

# pragma global_optimizer off // ?

static bool is_visible(nw4hbm::lyt::Pane *pPane)
{
	if (!pPane->IsVisible())
		return false;

	if (!pPane->GetParent())
		return true;

	return is_visible(pPane->GetParent());
}

#pragma pop

#pragma push

# pragma global_optimizer off

bool PaneComponent::isVisible()
{
	return is_visible(mpPane);
}

#pragma pop

} // namespace gui
} // namespace homebutton
