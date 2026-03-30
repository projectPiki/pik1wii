#ifndef EGG_GFX_TEXTURE_H
#define EGG_GFX_TEXTURE_H

#include <egg/egg_types.h>
#include <egg/core/eggHeap.h>
#include <egg/gfx/eggPalette.h>
#include <RevoSDK/gx.h>

namespace EGG {

struct ResTIMG {
	u8 mTextureFormat;              // _00
	u8 mAlphaEnabled; 				// _01
	u16 mSizeX;                     // _02
	u16 mSizeY;                     // _04
	u8 mWrapS;                      // _06
	u8 mWrapT;                      // _07
	u8 mPaletteFormat;              // _08
	u8 mColorFormat;                // _09
	u16 mPaletteEntryCount;         // _0A
	u32 mPaletteOffset;             // _0C
	GXBool mIsMIPmapEnabled;        // _10
	GXBool mDoEdgeLOD;              // _11
	GXBool mIsBiasClamp;            // _12
	GXBool mIsMaxAnisotropy;        // _13
	u8 mMinFilterType;              // _14
	u8 mMagFilterType;              // _15
	s8 mMinLOD;                     // _16
	s8 mMaxLOD;                     // _17
	u8 mTotalImageCount;            // _18
	u8 _19;                         // _19, unknown
	s16 mLODBias;                   // _1A
	s32 mImageDataOffset;           // _1C
};

enum TextureFlags {
	TEXFLAG_Unk1 = 0x1,
	TEXFLAG_Unk2 = 0x2,
};

class Texture {
public:
    Texture() {
        setCaptureFlag(false);
        mEmbPalette = nullptr;
        mTexInfo = nullptr;
    }

    Texture(const ResTIMG *p_timg, u8 param_1) {
        mEmbPalette = nullptr;
        storeTIMG(p_timg, param_1);
        setCaptureFlag(false);
    }

    void store(const ResTIMG *p_timg) {
        mTexInfo = nullptr;
        mEmbPalette = nullptr;
        mFlags = 0;
        mImage = nullptr;
        storeTIMG(p_timg, (u8)0);
    }

    Texture(int, int, GXTexFmt, Heap *);
    ~Texture();

    void storeTIMG(ResTIMG const *pTIMG, u8 param0);
    void storeTIMG(ResTIMG const *pTIMG, Palette *pPalette);
    void storeTIMG(ResTIMG const *pTIMG, Palette *pPalette, GXTlut tlut);
    void attachPalette(Palette *pPalette);
    void init();
    void initTexObj();
    void initTexObj(GXTlut tlut);
    void load(GXTexMapID texMapId);
    void capture(int, int, GXTexFmt, bool, u8);
    void captureTexture(int, int, int, int, bool, GXTexFmt, GXTexFmt);
    void captureDolTexture(int, int, int, int, bool, GXTexFmt);

    const ResTIMG *getTexInfo() const {
        return mTexInfo;
    }
    u8 getFormat() const {
        return mTexInfo->mTextureFormat;
    }
    s32 getTransparency() {
        return mTexInfo->mAlphaEnabled;
    }
    s32 getWidth() const {
        return mTexInfo->mSizeX;
    }
    s32 getHeight() const {
        return mTexInfo->mSizeY;
    }
    void setCaptureFlag(bool flag) {
        mFlags &= 2 | flag;
    }
    u8 getCaptureFlag() const {
        return mFlags & 1;
    }
    u8 getEmbPaletteDelFlag() const {
        return mFlags & 2;
    }
    void setEmbPaletteDelFlag(bool flag) {
        mFlags = (mFlags & 1) | (flag << 1);
    }
    bool operator==(const Texture &other) {
        return mTexInfo == other.mTexInfo && mpPalette == other.mpPalette && mWrapS == other.mWrapS &&
               mWrapT == other.mWrapT && mMinFilter == other.mMinFilter && mMagFilter == other.mMagFilter &&
               mMinLOD == other.mMinLOD && mMinLOD == other.mMinLOD && mLODBias == other.mLODBias;
    }
    bool operator!=(const Texture &other) {
        return !operator==(other);
    }

private:
    GXTexObj mTexObj; 			// _00
    const ResTIMG *mTexInfo; 	// _20
    void *mTexData; 			// _24
    Palette *mEmbPalette;		// _28
    Palette *mpPalette; 		// _2C
    u8 mWrapS; 					// _30
    u8 mWrapT; 					// _31
    u8 mMinFilter; 				// _32
    u8 mMagFilter; 				// _33
    u16 mMinLOD; 				// _34
    u16 mMaxLOD; 				// _36
    s16 mLODBias; 				// _38
    u8 mTlutName; 				// _3A
    u8 mFlags; 					// _3B
    ResTIMG* mImage; 			// _3C
};

} // namespace EGG

#endif