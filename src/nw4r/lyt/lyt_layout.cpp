#include <nw4r/lyt.h>
#include <nw4r/ut.h>

/******************************************************************************
 *
 * Utility functions
 *
 ******************************************************************************/
namespace nw4r {
namespace lyt {

MEMAllocator *Layout::mspAllocator;

namespace {



void SetTagProcessorImpl(Pane* pPane, ut::TagProcessorBase<wchar_t> *pTagProcessor) {
    TextBox* pTextBox = ut::DynamicCast<TextBox*>(pPane);
    if (pTextBox != nullptr) {
        pTextBox->SetTagProcessor(pTagProcessor);
    }

    NW4R_UT_LINKLIST_FOREACH (it, pPane->GetChildList(), {
        SetTagProcessorImpl(&*it, pTagProcessor); 
	})
}

 

bool IsIncludeAnimationGroupRef(
    GroupContainer *pGroupContainer, const AnimationGroupRef *groupRefs, u16 bindGroupNum, bool bDescendingBind,
    Pane *pTargetPane
) {
    for (u16 grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
        Group *pGroup = pGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
        NW4R_UT_LINKLIST_FOREACH (it, pGroup->GetPaneList(),{
            const Pane *t = it->mTarget;
            if (t == pTargetPane) {
                return true;
            }
            if (bDescendingBind) {
                for (const Pane *pParentPane = pTargetPane->GetParent(); pParentPane != nullptr;
                     pParentPane = pParentPane->GetParent()) {
                    if (t == pParentPane) {
                        return true;
                    }
                }
            }
        })
    }
    return false;
}
}

/******************************************************************************
 *
 * Layout
 *
 ******************************************************************************/

Layout::Layout()
    : mpRootPane(nullptr),
      mpGroupContainer(nullptr),
      mLayoutSize(0.0f, 0.0f){}

Layout::~Layout() {
    DeleteObj(mpGroupContainer);

    if (mpRootPane != nullptr && !mpRootPane->IsUserAllocated()) {
        DeleteObj(mpRootPane);
    }

    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mAnimTransList, {
        mAnimTransList.Erase(it);
        DeleteObj(&*it);
    })
}

bool Layout::Build(const void* pLytBinary, ResourceAccessor* pResAcsr) {
    const res::BinaryFileHeader* const pHeader =
        static_cast<const res::BinaryFileHeader*>(pLytBinary);

    if (!detail::TestFileHeader(*pHeader, SIGNATURE)) {
        return false;
    }
	
	if (!detail::TestFileVersion(*pHeader)) {
        return false;
    }

    ResBlockSet blockSet = {
        nullptr,     // pTextureList
        nullptr,     // pFontList
        nullptr,     // pMaterialList
        pResAcsr // pResAccessor
    };

    Pane* pParentPane = nullptr;
    Pane* pLastPane = nullptr;

    bool bReadRootGroup = false;
    int groupNestLevel = 0;

    const void* pBlockData =
        static_cast<const u8*>(pLytBinary) + pHeader->headerSize;

    for (int i = 0; i < pHeader->dataBlocks; i++) {
        const res::DataBlockHeader* pBlockHeader =
            static_cast<const res::DataBlockHeader*>(pBlockData);

        switch (detail::GetSignatureInt(pBlockHeader->kind)) {
        case res::Layout::SIGNATURE: {
            const res::Layout* pRes =
                static_cast<const res::Layout*>(pBlockData);

            mLayoutSize = pRes->layoutSize;
            break;
        }

        case SIGNATURE_TEXTURELIST: {
            blockSet.pTextureList =
                static_cast<const res::TextureList*>(pBlockData);
            break;
        }

        case SIGNATURE_FONTLIST: {
            blockSet.pFontList = static_cast<const res::FontList*>(pBlockData);
            break;
        }

        case SIGNATURE_MATERIALLIST: {
            blockSet.pMaterialList =
                static_cast<const res::MaterialList*>(pBlockData);
            break;
        }

        case res::Pane::SIGNATURE:
        case res::Picture::SIGNATURE:
        case res::TextBox::SIGNATURE:
        case res::Window::SIGNATURE:
        case res::Bounding::SIGNATURE: {
            Pane* pPane =
                BuildPaneObj(detail::GetSignatureInt(pBlockHeader->kind),
                             pBlockData, blockSet);

            if (pPane != nullptr) {
                if (mpRootPane == nullptr) {
                    mpRootPane = pPane;
                }

                if (pParentPane != nullptr) {
                    pParentPane->AppendChild(pPane);
                }

                pLastPane = pPane;
            }

            break;
        }
		
		case 'usd1': {
            pLastPane->SetExtUserDataList(static_cast<const res::ExtUserDataList*>(pBlockData));
            break;
		}

        case SIGNATURE_PANESTART: {
            pParentPane = pLastPane;
            break;
        }

        case SIGNATURE_PANEEND: {
            pLastPane = pParentPane;
            pParentPane = pLastPane->GetParent();
            break;
        }

        case res::Group::SIGNATURE: {
            if (!bReadRootGroup) {
                bReadRootGroup = true;
                mpGroupContainer = NewObj<GroupContainer>();
                break;
            }

            if (mpGroupContainer != nullptr && groupNestLevel == 1) {
                Group* pGroup = NewObj<Group>(
                    reinterpret_cast<const res::Group*>(pBlockHeader),
                    mpRootPane);

                if (pGroup != nullptr) {
                    mpGroupContainer->AppendGroup(pGroup);
                }
            }

            break;
        }

        case SIGNATURE_GROUPSTART: {
            groupNestLevel++;
            break;
        }

        case SIGNATURE_GROUPEND: {
            groupNestLevel--;
            break;
        }
        }

        pBlockData = static_cast<const u8*>(pBlockData) + pBlockHeader->size;
    }

    return true;
}

AnimTransform *Layout::CreateAnimTransform() {
    AnimTransformBasic *pAnimTrans = NewObj<AnimTransformBasic>();
    if (pAnimTrans) {
        mAnimTransList.PushBack(pAnimTrans);
    }
    return pAnimTrans;
}

AnimTransform *Layout::CreateAnimTransform(const void *animResBuf, ResourceAccessor *pResAcsr) {
    return CreateAnimTransform(AnimResource(animResBuf), pResAcsr);
}

AnimTransform *Layout::CreateAnimTransform(const AnimResource &animRes, ResourceAccessor *pResAcsr) {
    const res::AnimationBlock *pAnimBlock = animRes.GetResourceBlock();
    if (!pAnimBlock) {
        return nullptr;
    }

    AnimTransform *pAnimTrans = CreateAnimTransform();
    if (pAnimTrans) {
        pAnimTrans->SetResource(pAnimBlock, pResAcsr);
    }

    return pAnimTrans;
}

void Layout::BindAnimation(AnimTransform* pAnimTrans) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->BindAnimation(pAnimTrans, true, false);
}

void Layout::UnbindAnimation(AnimTransform* pAnimTrans) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->UnbindAnimation(pAnimTrans, true);
}

void Layout::UnbindAllAnimation() {
    UnbindAnimation(nullptr);
}

bool Layout::BindAnimationAuto(const AnimResource &animRes, ResourceAccessor *pResAcsr) {
    // Ensure Root pane and Resource Block Exists
    if (!mpRootPane) {
        return false;
    }
    if (!animRes.GetResourceBlock()) {
        return false;
    }

    AnimTransform *pAnimTrans = CreateAnimTransform();
    u16 bindGroupNum = animRes.GetGroupNum();
    u16 animNum = 0;
    if (bindGroupNum == 0) {
        // No Groups to bind with, only bind to root pane
        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animRes.GetResourceBlock()->animContNum);
        mpRootPane->BindAnimation(pAnimTrans, true, true);
    } else {
        // Bind to all Groups
        const AnimationGroupRef *groupRefs = animRes.GetGroupArray();
        for (int grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
            Group *pGroup = mpGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
            animNum += animRes.CalcAnimationNum(pGroup, animRes.IsDescendingBind());
        }
        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animNum);
        for (int grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
            Group *pGroup = mpGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
            lyt::BindAnimation(pGroup, pAnimTrans, animRes.IsDescendingBind(), true);
        }
    }
    u16 animSharInfoNum = animRes.GetAnimationShareInfoNum();
    if (animSharInfoNum != 0) {
        const AnimationShareInfo *animSharInfoAry = animRes.GetAnimationShareInfoArray();
        for (int i = 0; i < animSharInfoNum; i++) {
            Pane *pSrcPane = mpRootPane->FindPaneByName(animSharInfoAry[i].GetSrcPaneName(), true);
            detail::AnimPaneTree animPaneTree = detail::AnimPaneTree(pSrcPane, animRes);
            Group *pGroup = mpGroupContainer->FindGroupByName(animSharInfoAry[i].GetTargetGroupName());
            NW4R_UT_LINKLIST_FOREACH (it, pGroup->GetPaneList(),{
                if (it->mTarget != pSrcPane) {
                    if (bindGroupNum != 0) {
                        bool bInclude = IsIncludeAnimationGroupRef(
                            mpGroupContainer, animRes.GetGroupArray(), bindGroupNum, animRes.IsDescendingBind(),
                            it->mTarget);
                        if (!bInclude) {
                            continue;
                        }
                    }
                    animPaneTree.Bind(this, it->mTarget, pResAcsr);
                }
            })
        }
    }
    return true;
}

void Layout::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->SetAnimationEnable(pAnimTrans, enable, true);
}

void Layout::CalculateMtx(const DrawInfo& rInfo) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->CalculateMtx(rInfo);
}

void Layout::Draw(const DrawInfo& rInfo) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->Draw(rInfo);
}

void Layout::Animate(u32 option) {
    if (mpRootPane == nullptr) {
        return;
    }

    mpRootPane->Animate(option);
}

ut::Rect Layout::GetLayoutRect() const {
    return ut::Rect(-mLayoutSize.width / 2, mLayoutSize.height / 2, mLayoutSize.width / 2, -mLayoutSize.height / 2);
}

void Layout::SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor) {
    SetTagProcessorImpl(mpRootPane, pTagProcessor);
}

Pane* Layout::BuildPaneObj(s32 kind, const void* pBinary,
                           const ResBlockSet& rBlockSet) {

    switch (kind) {
    case res::Pane::SIGNATURE: {
        const res::Pane* pRes = static_cast<const res::Pane*>(pBinary);
        return NewObj<Pane>(pRes);
    } break; // required to not inline in Layout::Build

    case res::Picture::SIGNATURE: {
        const res::Picture* pRes = static_cast<const res::Picture*>(pBinary);
        return NewObj<Picture>(pRes, rBlockSet);
    } break; // required to not inline in Layout::Build

    case res::TextBox::SIGNATURE: {
        const res::TextBox* pRes = static_cast<const res::TextBox*>(pBinary);
        return NewObj<TextBox>(pRes, rBlockSet);
    } break; // required to not inline in Layout::Build

    case res::Window::SIGNATURE: {
        const res::Window* pRes = static_cast<const res::Window*>(pBinary);
        return NewObj<Window>(pRes, rBlockSet);
    }

    case res::Bounding::SIGNATURE: {
        const res::Bounding* pRes = static_cast<const res::Bounding*>(pBinary);
        return NewObj<Bounding>(pRes, rBlockSet);
    }

    default: {
        return nullptr;
    }
    }
}

} // namespace lyt
} // namespace nw4r