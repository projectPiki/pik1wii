#ifndef NW4R_LYT_TEX_MAP_H
#define NW4R_LYT_TEX_MAP_H
#include <nw4r/types_nw4r.h>

#include <RevoSDK/GX.h>
#include <RevoSDK/TPL.h>

namespace nw4r {
namespace lyt {

class TexMap {
public:
    TexMap() {
        SetImage(nullptr);
        SetSize(0, 0);
        SetTexelFormat(GX_TF_I4);
        SetWrapMode(GX_CLAMP, GX_CLAMP);
        SetMipMap(false);
        SetFilter(GX_LINEAR, GX_LINEAR);
        SetLOD(0.0f, 0.0f);
        SetLODBias(0.0f);
        SetBiasClampEnable(false);
        SetEdgeLODEnable(false);
        SetAnisotropy(GX_ANISO_1);
        SetPalette(nullptr);
        SetPaletteFormat(GX_TL_IA8);
        SetPaletteEntryNum(0);
    }

    TexMap(TPLPalette* pPalette, u32 id) {
        Set(pPalette, id);
        SetBiasClampEnable(false);
        SetAnisotropy(GX_ANISO_1);
    }

    void Get(GXTexObj* pTexObj) const;
    void Get(GXTlutObj* pTlutObj) const;

    void Set(const TexMap& rOther) {
        *this = rOther;
    }
    void Set(TPLPalette* pPalette, u32 id);
    void Set(const TPLDescriptor* pDesc);

    void SetNoWrap(const TexMap& rOther);
    void SetNoWrap(const TPLDescriptor* pDesc);
	
	void ReplaceImage(const TPLDescriptor*);
    void ReplaceImage(TPLPalette*, u32);

    void* GetImage() const {
        return mpImage;
    }
    void SetImage(void* pImage) {
        mpImage = pImage;
    }

    void* GetPalette() const {
        return mpPalette;
    }
    void SetPalette(void* pPalette) {
        mpPalette = pPalette;
    }

    u16 GetWidth() const {
        return mWidth;
    }
    u16 GetHeight() const {
        return mHeight;
    }
    Size GetSize() const {
        return Size(static_cast<f32>(mWidth), static_cast<f32>(mHeight));
    }
    void SetSize(u16 width, u16 height) {
        mWidth = width;
        mHeight = height;
    }

    f32 GetMinLOD() const {
        return mMinLOD;
    }
    f32 GetMaxLOD() const {
        return mMaxLOD;
    }
    void SetLOD(f32 minLOD, f32 maxLOD) {
        mMinLOD = minLOD;
        mMaxLOD = maxLOD;
    }

    f32 GetLODBias() const {
        return mLODBias / 256.0f;
    }
    void SetLODBias(f32 bias) {
        mLODBias = static_cast<u16>(bias * 256.0f);
    }

    u16 GetPaletteEntryNum() const {
        return mPaletteEntryNum;
    }
    void SetPaletteEntryNum(u16 entrynum) {
        mPaletteEntryNum = entrynum;
    }

    GXTexFmt GetTexelFormat() const {
        return static_cast<GXTexFmt>(mBits.textureFormat);
    }
    void SetTexelFormat(GXTexFmt format) {
        mBits.textureFormat = format;
    }

    bool IsMipMap() const {
        return mBits.mipmap;
    }
    void SetMipMap(bool mipmap) {
        mBits.mipmap = mipmap;
    }

    GXTexWrapMode GetWrapModeS() const {
        return static_cast<GXTexWrapMode>(mBits.wrapS);
    }
    GXTexWrapMode GetWrapModeT() const {
        return static_cast<GXTexWrapMode>(mBits.wrapT);
    }
    void SetWrapMode(GXTexWrapMode wrapS, GXTexWrapMode wrapT) {
        mBits.wrapS = wrapS;
        mBits.wrapT = wrapT;
    }

    GXTexFilter GetMinFilter() const {
        return static_cast<GXTexFilter>(mBits.minFilter);
    }
    GXTexFilter GetMagFilter() const {
        return static_cast<GXTexFilter>(mBits.magFilter);
    }
    void SetFilter(GXTexFilter minFilt, GXTexFilter magFilt) {
        mBits.minFilter = minFilt;
        mBits.magFilter = magFilt;
    }

    bool IsBiasClampEnable() const {
        return mBits.biasClampEnable;
    }
    void SetBiasClampEnable(bool enable) {
        mBits.biasClampEnable = enable;
    }

    bool IsEdgeLODEnable() const {
        return mBits.edgeLODEnable;
    }
    void SetEdgeLODEnable(bool enable) {
        mBits.edgeLODEnable = enable;
    }

    GXAnisotropy GetAnisotropy() const {
        return static_cast<GXAnisotropy>(mBits.anisotropy);
    }
    void SetAnisotropy(GXAnisotropy aniso) {
        mBits.anisotropy = aniso;
    }

    GXTlutFmt GetPaletteFormat() const {
        return static_cast<GXTlutFmt>(mBits.paletteFormat);
    }
    void SetPaletteFormat(GXTlutFmt format) {
        mBits.paletteFormat = format;
    }

private:
    void* mpImage;       	 // _00
    void* mpPalette;     	 // _04
    u16 mWidth;         	 // _08
    u16 mHeight;        	 // _0A
    f32 mMinLOD;        	 // _0C
    f32 mMaxLOD;         	 // _10
    u16 mLODBias;        	 // _14
    u16 mPaletteEntryNum; 	 // _16
    struct {
        u32 textureFormat : 4;
        u32 mipmap : 1;
        u32 wrapS : 2;
        u32 wrapT : 2;
        u32 minFilter : 3;
        u32 magFilter : 1;
        u32 biasClampEnable : 1;
        u32 edgeLODEnable : 1;
        u32 anisotropy : 2;
        u32 paletteFormat : 2;
    } mBits; 				 // _18
};

} // namespace lyt
} // namespace nw4r

#endif
