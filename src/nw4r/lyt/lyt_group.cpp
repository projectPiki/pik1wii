#include <nw4r/lyt.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

/******************************************************************************
 *
 * Group
 *
 ******************************************************************************/
Group::Group(const res::Group* pRes, Pane* pRootPane) {
    Init();

    strncpy(mName, pRes->name, NW4R_RES_NAME_SIZE);
    mName[NW4R_RES_NAME_SIZE] = '\0';

    const char* pNameBase =
        detail::ConvertOffsToPtr<char>(pRes, sizeof(res::Group));

    for (int i = 0; i < pRes->paneNum; i++) {
        Pane* pResult = pRootPane->FindPaneByName(
            pNameBase + i * NW4R_RES_NAME_SIZE, true);

        if (pResult != nullptr) {
            AppendPane(pResult);
        }
    }
}

void Group::Init() {
    mbUserAllocated = false;
}

Group::~Group() {
    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mPaneLinkList, {
        mPaneLinkList.Erase(it);
		Layout::DeleteObj<detail::PaneLink>(&*it);
    })
}

void Group::AppendPane(Pane* pPane) {
	detail::PaneLink *pPaneLink = Layout::NewObj<detail::PaneLink>();
    if (pPaneLink) {
        pPaneLink->mTarget = pPane;
        mPaneLinkList.PushBack(pPaneLink);
    }
}

/******************************************************************************
 *
 * GroupContainer
 *
 ******************************************************************************/
GroupContainer::~GroupContainer() {
    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mGroupList, {
        mGroupList.Erase(it);
        
        if (!it->IsUserAllocated()) {
            Layout::DeleteObj<Group>(&*it);
        }
    })
}

void GroupContainer::AppendGroup(Group* pGroup) {
    mGroupList.PushBack(pGroup);
}

Group* GroupContainer::FindGroupByName(const char* pName) {
    NW4R_UT_LINKLIST_FOREACH (it, mGroupList, {
        if (detail::EqualsResName(it->GetName(), pName)) {
            return &*it;
        }
    })

    return nullptr;
}

} // namespace lyt
} // namespace nw4r