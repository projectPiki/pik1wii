#include <nw4r/lyt.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(Picture, Pane);

Picture::Picture(const res::Picture* pRes, const ResBlockSet& rBlockSet)
    : Pane(pRes) {

    u8 num = ut::Min<u8>(pRes->texCoordNum, GX_MAX_TEXCOORD);
    Init(num);

    for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
        mVtxColors[i] = pRes->vtxCols[i];
    }

    if (num > 0 && !mTexCoordAry.IsEmpty()) {
        mTexCoordAry.Copy(
            reinterpret_cast<const u8*>(pRes) + sizeof(res::Picture), num);
    }

    const u32 *matOffsTbl = detail::ConvertOffsToPtr<u32>(rBlockSet.pMaterialList, sizeof(res::MaterialList));
    const res::Material *pResMaterial =
        detail::ConvertOffsToPtr<res::Material>(rBlockSet.pMaterialList, matOffsTbl[pRes->materialIdx]);
    mpMaterial = Layout::NewObj<Material>(pResMaterial, rBlockSet);
}

void Picture::Init(u8 num) {
    if (num > 0) {
        ReserveTexCoord(num);
    }
}

Picture::~Picture() {
    if (mpMaterial != nullptr && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
        mpMaterial = nullptr;
    }

    mTexCoordAry.Free();
}

void Picture::Append(const TexMap& rTexMap) {
    if (mpMaterial->GetTextureNum() >= mpMaterial->GetTextureCap() ||
        mpMaterial->GetTextureNum() >= mpMaterial->GetTexCoordGenCap()) {
        return;
    }

    u8 idx = mpMaterial->GetTextureNum();

    mpMaterial->SetTextureNum(idx + 1);
    mpMaterial->SetTexture(idx, rTexMap);

    mpMaterial->SetTexCoordGenNum(mpMaterial->GetTextureNum());
    mpMaterial->SetTexCoordGen(idx, TexCoordGen());

    SetTexCoordNum(mpMaterial->GetTextureNum());

    if (mSize == Size(0.0f, 0.0f) && mpMaterial->GetTextureNum() == 1) {
        mSize = detail::GetTextureSize(mpMaterial, 0);
    }
}

void Picture::ReserveTexCoord(u8 num) {
    mTexCoordAry.Reserve(num);
}

void Picture::SetTexCoordNum(u8 num) {
    mTexCoordAry.SetSize(num);
}

ut::Color Picture::GetVtxColor(u32 idx) const {
    return mVtxColors[idx];
}

void Picture::SetVtxColor(u32 idx, ut::Color color) {
    mVtxColors[idx] = color;
}

u8 Picture::GetVtxColorElement(u32 idx) const {
    return detail::GetVtxColorElement(mVtxColors, idx);
}

void Picture::SetVtxColorElement(u32 idx, u8 value) {
    detail::SetVtxColorElement(mVtxColors, idx, value);
}

void Picture::DrawSelf(const DrawInfo& rInfo) {
    if (mpMaterial == nullptr) {
        return;
    }

    LoadMtx(rInfo);

    bool useVtxColor = mpMaterial->SetupGX(
        detail::IsModulateVertexColor(mVtxColors, mGlbAlpha), mGlbAlpha);

    detail::SetVertexFormat(useVtxColor, mTexCoordAry.GetSize());

    detail::DrawQuad(GetVtxPos(), mSize, mTexCoordAry.GetSize(),
                     mTexCoordAry.GetArray(), useVtxColor ? mVtxColors : nullptr,
                     mGlbAlpha);
}

} // namespace lyt
} // namespace nw4r