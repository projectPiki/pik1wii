#ifndef NW4HBM_LYT_GROUP_H
#define NW4HBM_LYT_GROUP_H

#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include "homebuttonLib/nw4hbm/macros.h"
#include <types.h>


namespace nw4hbm { 
namespace lyt {

namespace res {
	
static u32 const SIGNATURE_GROUP_BLOCK = NW4HBM_FOUR_CHAR('g', 'r', 'p', '1');
static u32 const SIGNATURE_GROUP_BLOCK_START = NW4HBM_FOUR_CHAR('g', 'r', 's', '1');
static u32 const SIGNATURE_GROUP_BLOCK_END = NW4HBM_FOUR_CHAR('g', 'r', 'e', '1');

struct Group {
	static u32 const NAME_LENGTH = 16;

	DataBlockHeader	blockHeader;		// _00
	char name[NAME_LENGTH];				// _08
	u16 paneNum;						// _18
}; 
} // namespace res

class Group {
public:
	
	typedef ut::LinkList<Group, 4> LinkList;


	Group();
	Group(res::Group const *pResGroup, Pane *pRootPane);
		
	virtual ~Group(); // _08

	detail::PaneLink::LinkList &GetPaneList() { return mPaneLinkList; }
	char const *GetName() const { return mName; }
	bool IsUserAllocated() const { return mbUserAllocated; }
	void Init();
	void AppendPane(Pane *pPane);

	static u32 const NAME_LENGTH = res::Group::NAME_LENGTH;

private:
	ut::LinkListNode			mLink;				// _04
	detail::PaneLink::LinkList	mPaneLinkList;		// _0C
	char						mName[NAME_LENGTH];	// _18
	bool						mbUserAllocated;	// _28
};

class GroupContainer {
public:
	GroupContainer() {}	
	~GroupContainer();

	void AppendGroup(Group *pGroup);
	Group *FindGroupByName(char const *findName);

private:
	Group::LinkList	mGroupList;	// _00
};
} // namespace lyt
} // namespace nw4hbm

#endif // NW4HBM_LYT_GROUP_H
