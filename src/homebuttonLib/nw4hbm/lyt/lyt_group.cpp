#include "homebuttonLib/nw4hbm/lyt/group.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/layout.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/lyt/types.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include <string.h>
#include <new.h>
#include <types.h>

namespace nw4hbm { 
namespace lyt {

Group::Group()
{
	Init();
}

Group::Group(res::Group const *pResGroup, Pane *pRootPane)
{
	Init();
	memcpy(mName, pResGroup->name, sizeof mName);

	char const *paneName = detail::ConvertOffsToPtr<char>(pResGroup, sizeof *pResGroup);

	for (int i = 0; i < pResGroup->paneNum; ++i)
	{
		Pane *pFindPane = pRootPane->FindPaneByName(paneName + (int)NAME_LENGTH * i, true);

		if (pFindPane)
			AppendPane(pFindPane);
	}
}

void Group::Init()
{
	mbUserAllocated = false;
}

Group::~Group()
{
	NW4HBM_RANGE_FOR_NO_AUTO_INC(it, mPaneLinkList)
	{
		DECLTYPE(it) currIt = it++;

		mPaneLinkList.Erase(currIt);
		Layout::FreeMemory(&(*currIt));
	}
}

void Group::AppendPane(Pane *pPane)
{
	if (void *pMem = Layout::AllocMemory(sizeof(detail::PaneLink)))
	{
		detail::PaneLink *pPaneLink = new (pMem) detail::PaneLink;

		pPaneLink->mTarget = pPane;
		mPaneLinkList.PushBack(pPaneLink);
	}
}

GroupContainer::~GroupContainer()
{
	NW4HBM_RANGE_FOR_NO_AUTO_INC(it, mGroupList)
	{
		DECLTYPE(it) currIt = it++;

		mGroupList.Erase(currIt);

		if (!currIt->IsUserAllocated())
		{
			currIt->~Group();
			Layout::FreeMemory(&(*currIt));
		}
	}
}

void GroupContainer::AppendGroup(Group *pGroup)
{
	mGroupList.PushBack(pGroup);
}

Group *GroupContainer::FindGroupByName(char const *findName)
{
	NW4HBM_RANGE_FOR(it, mGroupList)
	{
		if (detail::EqualsPaneName(it->GetName(), findName))
			return &(*it);
	}

	return nullptr;
}

} // namespace lyt
} // namespace nw4hbm
