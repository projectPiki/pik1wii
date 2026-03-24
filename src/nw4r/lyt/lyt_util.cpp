#include <nw4r/lyt.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

void BindAnimation(Group *pGroup, AnimTransform *pAnimTrans, bool bRecursive, bool bDisable) {
	NW4R_UT_LINKLIST_FOREACH (it, pGroup->GetPaneList(), {
        it->mTarget->BindAnimation(pAnimTrans, bRecursive, bDisable);
    })
}

} // namespace lyt
} // namespace nw4r