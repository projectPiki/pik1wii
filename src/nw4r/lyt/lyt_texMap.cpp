#include <nw4r/lyt.h>

#include <RevoSDK/GX.h>
#include <RevoSDK/TPL.h>

namespace nw4r {
namespace lyt {

void TexMap::Get(GXTexObj* pTexObj) const {
    if (detail::IsCITexelFormat(GetTexelFormat())) {
        u32 tlut = GXGetTexObjTlut(pTexObj);

        GXInitTexObjCI(pTexObj, GetImage(), GetWidth(), GetHeight(),
                       (GXCITexFmt)GetTexelFormat(), GetWrapModeS(), GetWrapModeT(),
                       IsMipMap(), tlut);
    } else {
        GXInitTexObj(pTexObj, GetImage(), GetWidth(), GetHeight(),
                     GetTexelFormat(), GetWrapModeS(), GetWrapModeT(),
                     IsMipMap());
    }

    GXInitTexObjLOD(pTexObj, GetMinFilter(), GetMagFilter(), GetMinLOD(),
                    GetMaxLOD(), GetLODBias(), IsBiasClampEnable(),
                    IsEdgeLODEnable(), GetAnisotropy());
}

void TexMap::Get(GXTlutObj* pTlutObj) const {
    GXInitTlutObj(pTlutObj, GetPalette(), GetPaletteFormat(),
                  GetPaletteEntryNum());
}

void TexMap::Set(TPLPalette* pPalette, u32 id) {
    if (reinterpret_cast<u32>(pPalette->descriptorArray) < 0x80000000) {
        TPLBind(pPalette);
    }

    Set(TPLGet(pPalette, id));
}

void TexMap::ReplaceImage(const TPLDescriptor* pTPLDesc) {
    const TPLHeader& header = *pTPLDesc->textureHeader;
    mpImage = header.data;
    SetSize(header.width, header.height);
    SetTexelFormat(GXTexFmt(header.format));

    if (const TPLClutHeader* const pClut = pTPLDesc->CLUTHeader) {
        SetPalette(pClut->data);
        SetPaletteFormat(pClut->format);
        SetPaletteEntryNum(pClut->numEntries);
    } else {
        SetPalette(nullptr);
        SetPaletteFormat(GXTlutFmt(0));
        SetPaletteEntryNum(0);
    }
}

void TexMap::ReplaceImage(TPLPalette* p, u32 id) {
    if (reinterpret_cast<u32>(p->descriptorArray) < 0x80000000) {
        TPLBind(p);
    }

    ReplaceImage(TPLGet(p, id));
}

void TexMap::Set(const TPLDescriptor* pDesc) {
    SetNoWrap(pDesc);

    const TPLHeader& rTexHeader = *pDesc->textureHeader;
    SetWrapMode(rTexHeader.wrapS, rTexHeader.wrapT);
}

void TexMap::SetNoWrap(const TexMap& rOther) {
    GXTexWrapMode wrapS = GetWrapModeS();
    GXTexWrapMode wrapT = GetWrapModeT();

    Set(rOther);
    SetWrapMode(wrapS, wrapT);
}

void TexMap::SetNoWrap(const TPLDescriptor* pDesc) {
    const TPLHeader& rTexHeader = *pDesc->textureHeader;

    SetImage(rTexHeader.data);
    SetSize(rTexHeader.width, rTexHeader.height);
    SetTexelFormat(static_cast<GXTexFmt>(rTexHeader.format));

    SetMipMap(rTexHeader.minLOD != rTexHeader.maxLOD);
    SetFilter(rTexHeader.minFilter, rTexHeader.magFilter);

    SetLOD(static_cast<f32>(rTexHeader.minLOD),
           static_cast<f32>(rTexHeader.maxLOD));

    SetLODBias(rTexHeader.LODBias);
    SetEdgeLODEnable(rTexHeader.edgeLODEnable);

    const TPLClutHeader* const pClutHeader = pDesc->CLUTHeader;

    if (pClutHeader != NULL) {
        SetPalette(pClutHeader->data);
        SetPaletteFormat(pClutHeader->format);
        SetPaletteEntryNum(pClutHeader->numEntries);
    } else {
        SetPalette(NULL);
        SetPaletteFormat(GX_TL_IA8);
        SetPaletteEntryNum(0);
    }
}

} // namespace lyt
} // namespace nw4r