#ifndef NW4R_LYT_GROUP_H
#define NW4R_LYT_GROUP_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_resources.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

// Forward declarations
class Pane;

namespace detail {

/******************************************************************************
 *
 * PaneLink
 *
 ******************************************************************************/
struct PaneLink {
    NW4R_UT_LINKLIST_NODE_DECL(); // _04
    Pane* mTarget;                // _08
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(PaneLink);

} // namespace detail

namespace res {

/******************************************************************************
 *
 * GRP1 binary layout
 *
 ******************************************************************************/
struct Group {
    static const u32 SIGNATURE = FOURCC('g', 'r', 'p', '1');

    DataBlockHeader blockHeader;      // _00
    char name[NW4R_RES_NAME_SIZE];    // _08
    u16 paneNum;                      // _18
    u8 PADDING_0x1A[0x1C - 0x1A];     // _1A
};

} // namespace res

/******************************************************************************
 *
 * Group
 *
 ******************************************************************************/
class Group {
public:
    Group(const res::Group* pRes, Pane* pRootPane);
    virtual ~Group(); // _08

    void AppendPane(Pane* pPane);

    detail::PaneLinkList& GetPaneList() {
        return mPaneLinkList;
    }

    const char* GetName() const {
        return mName;
    }

    bool IsUserAllocated() const {
        return mbUserAllocated;
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL(); 			// _04

protected:
    detail::PaneLinkList mPaneLinkList;     // _0C
    char mName[NW4R_RES_NAME_SIZE + 1]; 	// _18
    bool mbUserAllocated;                   // _29
    u8 PADDING_0x2A[0x2C - 0x2A];           // _2A

private:
    void Init();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Group);

/******************************************************************************
 *
 * GroupContainer
 *
 ******************************************************************************/
class GroupContainer {
public:
    GroupContainer() {}
    ~GroupContainer();

    void AppendGroup(Group* pGroup);
    Group* FindGroupByName(const char* pName);

    GroupList& GetGroupList() {
        return mGroupList;
    }

protected:
    GroupList mGroupList; // _00
};

} // namespace lyt
} // namespace nw4r

#endif
