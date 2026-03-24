#include <nw4r/lyt.h>
#include <nw4r/ut.h>

/******************************************************************************
 *
 * Private structures
 *
 ******************************************************************************/
namespace nw4r {
namespace lyt {
namespace res {

/******************************************************************************
 * AnimationTarget
 ******************************************************************************/
struct AnimationTarget {
    enum AnimCurve {
        ANIMCURVE_NONE,
        ANIMCURVE_STEP,
        ANIMCURVE_HERMITE,

        ANIMCURVE_MAX
    };

    u8 id;                     // at 0x0
    u8 target;                 // at 0x1
    u8 curveType;              // at 0x2
    u8 PADDING_0x3;            // at 0x3
    u16 keyNum;                // at 0x4
    u8 PADDING_0x6[0x8 - 0x6]; // at 0x6
    u32 keysOffset;            // at 0x8
};

/******************************************************************************
 * StepKey
 ******************************************************************************/
struct StepKey {
    f32 frame;       // at 0x0
    u16 value;       // at 0x4
    u16 PADDING_0x6; // at 0x6
};
/******************************************************************************
 * HermiteKey
 ******************************************************************************/
struct HermiteKey {
    f32 frame; // at 0x0
    f32 value; // at 0x4
    f32 slope; // at 0x8
};

} // namespace res
} // namespace lyt
} // namespace nw4r

/******************************************************************************
 *
 * Utility functions
 *
 ******************************************************************************/
namespace nw4r {
namespace lyt {
namespace {

inline bool RIsSame(f32 a, f32 b, f32 tolerance) {
    f32 c = a - b;
    return -tolerance < c && c < tolerance;
}

u16 GetStepCurveValue(f32 frame, const res::StepKey* pKeys, u32 numKey) {
    if (numKey == 1 || frame <= pKeys[0].frame) {
        return pKeys[0].value;
    }

    if (frame >= pKeys[numKey - 1].frame) {
        return pKeys[numKey - 1].value;
    }

    int left = 0;
    int right = numKey - 1;

    while (left != right - 1 && left != right) {
        int center = (left + right) / 2;
        const res::StepKey& rCenterKey = pKeys[center];

        if (frame < rCenterKey.frame) {
            right = center;
        } else {
            left = center;
        }
    }

    if (RIsSame(frame, pKeys[right].frame, 0.001f)) {
        return pKeys[right].value;
    }

    return pKeys[left].value;
}

f32 GetHermiteCurveValue(f32 frame, const res::HermiteKey* pKeys, u32 numKey) {

    if (numKey == 1 || frame <= pKeys[0].frame) {
        return pKeys[0].value;
    }

    if (frame >= pKeys[numKey - 1].frame) {
        return pKeys[numKey - 1].value;
    }

    int left = 0;
    int right = numKey - 1;

    while (left != right - 1 && left != right) {
        int center = (left + right) / 2;

        if (frame <= pKeys[center].frame) {
            right = center;
        } else {
            left = center;
        }
    }

    const res::HermiteKey& rLeftKey = pKeys[left];
    const res::HermiteKey& rRightKey = pKeys[right];

    if (RIsSame(frame, rRightKey.frame, 0.001f)) {
        if (right < numKey - 1 && rRightKey.frame == pKeys[right + 1].frame) {
            return pKeys[right + 1].value;
        }

        return rRightKey.value;
    }

    f32 t1 = frame - rLeftKey.frame;
    f32 t2 = 1.0f / (rRightKey.frame - rLeftKey.frame);

    f32 v0 = rLeftKey.value;
    f32 v1 = rRightKey.value;

    f32 s0 = rLeftKey.slope;
    f32 s1 = rRightKey.slope;

    f32 t1_t1_t2 = t1 * t1 * t2;
    f32 t1_t1_t2_t2 = t1_t1_t2 * t2;
    f32 t1_t1_t1_t2_t2 = t1 * t1_t1_t2_t2;
    f32 t1_t1_t1_t2_t2_t2 = t1_t1_t1_t2_t2 * t2;

    return v0 * (2.0f * t1_t1_t1_t2_t2_t2 - 3.0f * t1_t1_t2_t2 + 1.0f) +
           v1 * (-2.0f * t1_t1_t1_t2_t2_t2 + 3.0f * t1_t1_t2_t2) +
           s0 * (t1_t1_t1_t2_t2 - 2.0f * t1_t1_t2 + t1) +
           s1 * (t1_t1_t1_t2_t2 - t1_t1_t2);
}


void AnimatePaneSRT(Pane* pPane, const res::AnimationInfo* pAnimInfo,
                    const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);

        const res::HermiteKey* pKeys =
            detail::ConvertOffsToPtr<res::HermiteKey>(pTarget,
                                                      pTarget->keysOffset);

        pPane->SetSRTElement(
            pTarget->target,
            GetHermiteCurveValue(frame, pKeys, pTarget->keyNum));
    }
}

void AnimateVisibility(Pane* pPane, const res::AnimationInfo* pAnimInfo,
                       const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);

        const res::StepKey* pKeys = detail::ConvertOffsToPtr<res::StepKey>(
            pTarget, pTarget->keysOffset);

        pPane->SetVisible(GetStepCurveValue(frame, pKeys, pTarget->keyNum) !=
                          0);
    }
}

void AnimateVertexColor(Pane* pPane, const res::AnimationInfo* pAnimInfo,
                        const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);
        const res::HermiteKey* pKeys =
            detail::ConvertOffsToPtr<res::HermiteKey>(pTarget,
                                                      pTarget->keysOffset);

        f32 fValue = GetHermiteCurveValue(frame, pKeys, pTarget->keyNum);
        fValue += 0.5f;

        u8 bValue;
        OSf32tou8(&fValue, &bValue);

        pPane->SetColorElement(pTarget->target, bValue);
    }
}

void AnimateMaterialColor(Material* pMaterial,
                          const res::AnimationInfo* pAnimInfo,
                          const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);
        const res::HermiteKey* pKeys =
            detail::ConvertOffsToPtr<res::HermiteKey>(pTarget,
                                                      pTarget->keysOffset);

        f32 fValue = GetHermiteCurveValue(frame, pKeys, pTarget->keyNum);
        fValue += 0.5f;

        s16 sValue;
        OSf32tos16(&fValue, &sValue);

        // [-1024, 1023]
        sValue = ut::Min<s16>(ut::Max<s16>(sValue, -1024), 1023);

        pMaterial->SetColorElement(pTarget->target, sValue);
    }
}

void AnimateTextureSRT(Material* pMaterial, const res::AnimationInfo* pAnimInfo,
                       const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);

        if (pTarget->id < pMaterial->GetTexSRTCap()) {
            const res::HermiteKey* pKeys =
                detail::ConvertOffsToPtr<res::HermiteKey>(pTarget,
                                                          pTarget->keysOffset);

            pMaterial->SetTexSRTElement(
                pTarget->id, pTarget->target,
                GetHermiteCurveValue(frame, pKeys, pTarget->keyNum));
        }
    }
}

void AnimateTexturePattern(Material* pMaterial,
                           const res::AnimationInfo* pAnimInfo,
                           const u32* pTargetOffsetTbl, f32 frame,
                           void** ppTexPalettes) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);

        if (pTarget->id < pMaterial->GetTextureNum()) {
            const res::StepKey* pKeys = detail::ConvertOffsToPtr<res::StepKey>(
                pTarget, pTarget->keysOffset);

            u16 idx = GetStepCurveValue(frame, pKeys, pTarget->keyNum);

             if (ppTexPalettes[idx] != nullptr) {
                pMaterial->GetTexturePtr(pTarget->id)->ReplaceImage((TPLPalette *)ppTexPalettes[idx], 0);
			}
        }
    }
}

void AnimateIndTexSRT(Material* pMaterial, const res::AnimationInfo* pAnimInfo,
                      const u32* pTargetOffsetTbl, f32 frame) {

    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pTarget =
            detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo,
                                                           pTargetOffsetTbl[i]);

        if (pTarget->id < pMaterial->GetIndTexSRTCap()) {
            const res::HermiteKey* pKeys =
                detail::ConvertOffsToPtr<res::HermiteKey>(pTarget,
                                                          pTarget->keysOffset);

            pMaterial->SetIndTexSRTElement(
                pTarget->id, pTarget->target,
                GetHermiteCurveValue(frame, pKeys, pTarget->keyNum));
        }
    }
}

bool IsBindAnimation(Material *pMaterial, AnimTransform *pAnimTrans) {
    return false;

    if (pMaterial->FindAnimationLink(pAnimTrans)) {
        return true;
    }
    return false;
}

bool IsBindAnimation(Pane *pPane, AnimTransform *pAnimTrans) {
    return false;

    if (pPane->FindAnimationLinkSelf(pAnimTrans)) {
        return true;
    }
    return false;
}

} // namespace

/******************************************************************************
 *
 * AnimTransform
 *
 ******************************************************************************/
AnimTransform::AnimTransform() : mpRes(nullptr), mFrame(0.0f) {}

AnimTransform::~AnimTransform() {}

u16 AnimTransform::GetFrameSize() const {
    return mpRes->frameSize;
}

bool AnimTransform::IsLoopData() const {
    return mpRes->loop != 0;
}

/******************************************************************************
 *
 * AnimTransformBasic
 *
 ******************************************************************************/
AnimTransformBasic::AnimTransformBasic()
    : mpFileResAry(nullptr), mAnimLinkAry(nullptr), mAnimLinkNum(0) {}

AnimTransformBasic::~AnimTransformBasic() {
    if (mAnimLinkAry != nullptr) {
        Layout::FreeMemory(mAnimLinkAry);
    }

    if (mpFileResAry != nullptr) {
        Layout::FreeMemory(mpFileResAry);
    }
}

void AnimTransformBasic::SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor) {
    SetResource(pRes, pResAccessor, pRes->animContNum);
}

void AnimTransformBasic::SetResource(const res::AnimationBlock* pBlock,
                                     ResourceAccessor* pAccessor, u16 animNum) {
    mpRes = pBlock;
    mpFileResAry = nullptr;

    if (mpRes->fileNum > 0) {
        mpFileResAry = Layout::NewArray<void*>(mpRes->fileNum);

        if (mpFileResAry != nullptr) {
            const u32* pStrTable = detail::ConvertOffsToPtr<u32>(
                mpRes, sizeof(res::AnimationBlock));

            for (int i = 0; i < mpRes->fileNum; i++) {
                const char* pName = detail::GetStrTableStr(pStrTable, i);

                mpFileResAry[i] = pAccessor->GetResource(
                    ArcResourceAccessor::RES_TYPE_TEXTURE, pName, nullptr);
            }
        }
    }

    mAnimLinkAry = Layout::NewArray<AnimationLink>(animNum);
    if (mAnimLinkAry) {
        mAnimLinkNum = animNum;
    }
}

void AnimTransformBasic::Bind(Pane* pPane, bool recursive, bool disable) {
    AnimationLink *pCrAnimLink = nullptr;
	const u32* const pContentOffsetTbl =
        detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset);

    for (u16 i = 0; i < mpRes->animContNum; i++) {
        const res::AnimationContent& rContent =
            *detail::ConvertOffsToPtr<res::AnimationContent>(
                mpRes, pContentOffsetTbl[i]);

        if (rContent.type == res::AnimationContent::ANIMTYPE_PANE) {
            Pane* pResult = pPane->FindPaneByName(rContent.name, recursive);

            if (pResult && !IsBindAnimation(pResult, this)) {
                pCrAnimLink = Bind(pResult, pCrAnimLink, i, disable);
                if (pCrAnimLink == nullptr) {
                    break;
                }
            }
        } else /* res::AnimationContent::ANIMTYPE_MATERIAL */ {
            Material* pResult =
                pPane->FindMaterialByName(rContent.name, recursive);

            if (pResult && !IsBindAnimation(pResult, this)) {
                pCrAnimLink = Bind(pResult, pCrAnimLink, i, disable);
                if (pCrAnimLink == nullptr) {
                    break;
                }
            }
        }
    }
}

void AnimTransformBasic::Bind(Material* pMaterial, bool disable) {
	AnimationLink *pCrAnimLink = nullptr;
    const u32* const pContentOffsetTbl =
        detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset);

    for (u16 i = 0; i < mpRes->animContNum; i++) {
        const res::AnimationContent& rContent =
            *detail::ConvertOffsToPtr<res::AnimationContent>(
                mpRes, pContentOffsetTbl[i]);

        if (rContent.type == res::AnimationContent::ANIMTYPE_MATERIAL){
			if (detail::EqualsMaterialName(pMaterial->GetName(), rContent.name) && !IsBindAnimation(pMaterial, this)) {
                pCrAnimLink = Bind(pMaterial, pCrAnimLink, i, disable);
                if (pCrAnimLink == nullptr) {
                    break;
                }
            }
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Pane* pPane) {
    u32 contentOffset =
        detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset)[idx];

    const res::AnimationContent* pContent =
        detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, contentOffset);

    const u32* pInfoOffsetTbl =
        detail::ConvertOffsToPtr<u32>(pContent, sizeof(res::AnimationContent));

    for (int i = 0; i < pContent->num; i++) {
        const res::AnimationInfo* pAnimInfo =
            detail::ConvertOffsToPtr<res::AnimationInfo>(pContent,
                                                         pInfoOffsetTbl[i]);

        const u32* pTargetOffsetTbl = detail::ConvertOffsToPtr<u32>(
            pAnimInfo, sizeof(res::AnimationInfo));

        switch (pAnimInfo->kind) {
        case res::AnimationInfo::SIGNATURE_ANMPANESRT: {
            AnimatePaneSRT(pPane, pAnimInfo, pTargetOffsetTbl, mFrame);
            break;
        }

        case res::AnimationInfo::SIGNATURE_ANMPANEVIS: {
            AnimateVisibility(pPane, pAnimInfo, pTargetOffsetTbl, mFrame);
            break;
        }

        case res::AnimationInfo::SIGNATURE_ANMVTXCLR: {
            AnimateVertexColor(pPane, pAnimInfo, pTargetOffsetTbl, mFrame);
            break;
        }
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Material* pMaterial) {
    u32 contentOffset =
        detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset)[idx];

    const res::AnimationContent* pContent =
        detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, contentOffset);

    const u32* pInfoOffsetTbl =
        detail::ConvertOffsToPtr<u32>(pContent, sizeof(res::AnimationContent));

    for (int i = 0; i < pContent->num; i++) {
        const res::AnimationInfo* pAnimInfo =
            detail::ConvertOffsToPtr<res::AnimationInfo>(pContent,
                                                         pInfoOffsetTbl[i]);

        const u32* pTargetOffsetTbl = detail::ConvertOffsToPtr<u32>(
            pAnimInfo, sizeof(res::AnimationInfo));

        switch (pAnimInfo->kind) {
        case res::AnimationInfo::SIGNATURE_ANMMATCLR: {
            AnimateMaterialColor(pMaterial, pAnimInfo, pTargetOffsetTbl,
                                 mFrame);
            break;
        }

        case res::AnimationInfo::SIGNATURE_ANMTEXSRT: {
            AnimateTextureSRT(pMaterial, pAnimInfo, pTargetOffsetTbl, mFrame);
            break;
        }

        case res::AnimationInfo::SIGNATURE_ANMTEXPAT: {
            if (mpFileResAry == nullptr) {
                break;
            }

            AnimateTexturePattern(pMaterial, pAnimInfo, pTargetOffsetTbl,
                                  mFrame, mpFileResAry);
            break;
        }

        case res::AnimationInfo::SIGNATURE_ANMINDTEXSRT: {
            AnimateIndTexSRT(pMaterial, pAnimInfo, pTargetOffsetTbl, mFrame);
            break;
        }
        }
    }
}


AnimationLink *AnimTransformBasic::FindUnbindLink(AnimationLink *pLink) const {
    if (pLink == nullptr) {
        pLink = mAnimLinkAry;
    }

    while (pLink < &mAnimLinkAry[mAnimLinkNum]) {
        if (pLink->GetAnimTransform() == nullptr) {
            return pLink;
        }
        pLink++;
    }

    return nullptr;
}

template <typename T>
AnimationLink *AnimTransformBasic::Bind(T *pTarget, AnimationLink *pAnimLink, u16 idx, bool bDisable) {
    pAnimLink = FindUnbindLink(pAnimLink);
    if (!pAnimLink) {
        return nullptr;
    }
    pAnimLink->Set(this, idx, bDisable);
    pTarget->AddAnimationLink(pAnimLink);
    pAnimLink++;
    return pAnimLink;
}

AnimResource::AnimResource() {
    Init();
}

void AnimResource::Set(const void *animResBuf) {
    Init();
    const res::BinaryFileHeader *pFileHeader = (res::BinaryFileHeader *)animResBuf;
    if (detail::TestFileHeader(*pFileHeader, 'RLAN')) {
        if (detail::TestFileVersion(*pFileHeader)) {
            mpFileHeader = pFileHeader;
            const res::DataBlockHeader *pDataBlockHead =
                detail::ConvertOffsToPtr<res::DataBlockHeader>(mpFileHeader, mpFileHeader->headerSize);
            for (int i = 0; i < mpFileHeader->dataBlocks; i++) {
                switch (detail::GetSignatureInt(pDataBlockHead->kind)) {
                    case 'pat1': mpTagBlock = (res::AnimationTagBlock *)pDataBlockHead; break;
                    case 'pah1': mpShareBlock = (res::AnimationShareBlock *)pDataBlockHead; break;
                    case 'pai1': mpResBlock = (res::AnimationBlock *)pDataBlockHead; break;
                }
                pDataBlockHead = detail::ConvertOffsToPtr<res::DataBlockHeader>(pDataBlockHead, pDataBlockHead->size);
            }
        }
    }
}

void AnimResource::Init() {
    mpFileHeader = nullptr;
    mpResBlock = nullptr;
    mpTagBlock = nullptr;
    mpShareBlock = nullptr;
}

u16 AnimResource::GetGroupNum() const {
    if (mpTagBlock) {
        return mpTagBlock->groupNum;
    }
    return 0;
}

const AnimationGroupRef *AnimResource::GetGroupArray() const {
    if (mpTagBlock) {
        const AnimationGroupRef *groups =
            detail::ConvertOffsToPtr<AnimationGroupRef>(mpTagBlock, mpTagBlock->groupsOffset);
        return groups;
    }
    return nullptr;
}

bool AnimResource::IsDescendingBind() const {
    if (mpTagBlock) {
        return detail::TestBit(mpTagBlock->flag, 0);
    }
    return false;
}

u16 AnimResource::GetAnimationShareInfoNum() const {
    if (mpShareBlock) {
        return mpShareBlock->shareNum;
    }
    return 0;
}

const AnimationShareInfo *AnimResource::GetAnimationShareInfoArray() const {
    if (mpShareBlock) {
        return detail::ConvertOffsToPtr<AnimationShareInfo>(mpShareBlock, mpShareBlock->animShareInfoOffset);
    }
    return 0;
}

u16 AnimResource::CalcAnimationNum(Pane *pPane, bool bRecursive) const {
    u16 linkNum = 0;
    const u32 *animContOffsets = detail::ConvertOffsToPtr<u32>(mpResBlock, mpResBlock->animContOffsetsOffset);
    for (u16 i = 0; i < mpResBlock->animContNum; i++) {
        const res::AnimationContent &animCont =
            *detail::ConvertOffsToPtr<res::AnimationContent>(mpResBlock, animContOffsets[i]);
        if (animCont.type == 0) {
            Pane *pFindPane = pPane->FindPaneByName(animCont.name, bRecursive);
            if (pFindPane) {
                linkNum++;
            }
        } else {
            Material *pFindMat = pPane->FindMaterialByName(animCont.name, bRecursive);
            if (pFindMat) {
                linkNum++;
            }
        }
    }
    return linkNum;
}

u16 AnimResource::CalcAnimationNum(Group *pGroup, bool bRecursive) const {
    u16 linkNum = 0;
    NW4R_UT_LINKLIST_FOREACH (it, pGroup->GetPaneList(), {
        linkNum += CalcAnimationNum(it->mTarget, bRecursive);
    })
    return linkNum;
}

namespace detail {

u16 AnimPaneTree::FindAnimContent(const res::AnimationBlock *pAnimBlock, const char *animContName, u8 animContType) {
    const u32 *animContOffsets = detail::ConvertOffsToPtr<u32>(pAnimBlock, pAnimBlock->animContOffsetsOffset);
    for (u16 i = 0; i < pAnimBlock->animContNum; i++) {
        const res::AnimationContent *pAnimCont =
            detail::ConvertOffsToPtr<res::AnimationContent>(pAnimBlock, animContOffsets[i]);
        if (pAnimCont->type == animContType && EqualsMaterialName(pAnimCont->name, animContName)) {
            return i;
        }
    }
    return -1;
}

void AnimPaneTree::Init() {
    mLinkNum = 0;
    mAnimPaneIdx = 0;
    mAnimMatCnt = 0;
    for (u8 i = 0; i < 9; i++) {
        mAnimMatIdxs[i] = 0;
    }
}

void AnimPaneTree::Set(Pane *pTargetPane, const AnimResource &animRes) {
    u16 linkNum = 0;
    const res::AnimationBlock *pAnimBlock = animRes.GetResourceBlock();

    u16 animContIdx = FindAnimContent(pAnimBlock, pTargetPane->GetName(), 0);
    if (animContIdx != 0xFFFF) {
        linkNum++;
    }

    u8 animMatCnt = pTargetPane->GetMaterialNum();
    u16 animMatIdxs[9];
    for (u8 i = 0; i < animMatCnt; i++) {
        animMatIdxs[i] = FindAnimContent(pAnimBlock, pTargetPane->GetMaterial(i)->GetName(), 1);
        if (animMatIdxs[i] != 0xFFFF) {
            linkNum++;
        }
    }
    if (linkNum != 0) {
        mAnimRes = animRes;
        mAnimPaneIdx = animContIdx;
        mAnimMatCnt = animMatCnt;
        for (u8 i = 0; i < animMatCnt; i++) {
            mAnimMatIdxs[i] = animMatIdxs[i];
        }
        mLinkNum = linkNum;
    }
}

AnimTransform *AnimPaneTree::Bind(Layout *pLayout, Pane *pTargetPane, ResourceAccessor *pResAccessor) const {
    AnimTransformBasic *pAnimTrans = (AnimTransformBasic *)pLayout->CreateAnimTransform();
    pAnimTrans->SetResource(mAnimRes.GetResourceBlock(), pResAccessor, mLinkNum);

    AnimationLink *pCrAnimLink = nullptr;
    if (mAnimPaneIdx != 0xFFFF) {
        pCrAnimLink = pAnimTrans->Bind(pTargetPane, nullptr, mAnimPaneIdx, true);
    }

    //u8 animMatMax = ut::Min<u8>(mAnimMatCnt, pTargetPane->GetMaterialNum());
    for (u8 i = 0; i < mAnimMatCnt; i++) {
        if (mAnimMatIdxs[i] != 0xFFFF) {
            Material *pMaterial = pTargetPane->GetMaterial();
            pCrAnimLink = pAnimTrans->Bind(pMaterial, pCrAnimLink, mAnimMatIdxs[i], true);
        }
	}

    return pAnimTrans;
}


AnimationLink* FindAnimationLink(AnimationLinkList* pAnimList,
                                 AnimTransform* pAnimTrans) {

    NW4R_UT_LINKLIST_FOREACH (it, *pAnimList, {
        if (pAnimTrans == it->GetAnimTransform()) {
            return &*it;
        }
    })

    return nullptr;
}

AnimationLink *FindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, const AnimResource &animRes) {
    NW4R_UT_LINKLIST_FOREACH (it, *pAnimList, {
        if (animRes.GetResourceBlock() == it->GetAnimTransform()->GetAnimResource()) {
            return &*it;
        }
    })
    return nullptr;
}

void UnbindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, AnimTransform *pAnimTrans) {
    ut::LinkList<AnimationLink, 0>::Iterator it = pAnimList->GetBeginIter();
    while (it != pAnimList->GetEndIter()) {
        ut::LinkList<AnimationLink, 0>::Iterator currIt = it++;
        if (pAnimTrans == nullptr || currIt->GetAnimTransform() == pAnimTrans) {
            pAnimList->Erase(currIt);
            currIt->Reset();
        }
    }
}

} // namespace detail
} // namespace lyt
} // namespace nw4r