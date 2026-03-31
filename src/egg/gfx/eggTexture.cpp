#include <egg/gfx/eggTexture.h>

namespace EGG {
	
Texture::Texture(int sizeX, int sizeY, GXTexFmt textureFormat, Heap* heap) {
    if (!heap) {
        heap = Heap::getCurrentHeap();
    }
	mFlags                  = (mFlags & TEXFLAG_Unk2) | TEXFLAG_Unk1;
	u32 texBufferSize       = GXGetTexBufferSize(sizeX, sizeY, textureFormat, GX_FALSE, 1);
	mImage                  = reinterpret_cast<ResTIMG*>(new (heap, 0x20) u8[texBufferSize + 0x20]);
	ResTIMG* img            = mImage;
	img->mTextureFormat     = textureFormat;
	img->mAlphaEnabled      = JUT_TRANSPARENCY_DISABLE;
	img->mSizeX             = sizeX;
	img->mSizeY             = sizeY;
	img->mWrapS             = GX_CLAMP;
	img->mWrapT             = GX_CLAMP;
	img->mPaletteFormat     = 0;
	img->mColorFormat       = GX_TL_IA8;
	img->mPaletteEntryCount = 0;
	img->mPaletteOffset     = 0;
	img->mIsMIPmapEnabled   = GX_FALSE;
	img->mDoEdgeLOD         = GX_FALSE;
	img->mIsBiasClamp       = GX_FALSE;
	img->mIsMaxAnisotropy   = GX_FALSE;
	img->mMinFilterType     = GX_LINEAR;
	img->mMagFilterType     = GX_LINEAR;
	img->mMinLOD            = 0;
	img->mMaxLOD            = 0;
	img->mTotalImageCount   = 1;
	img->mLODBias           = 0;
	img->mImageDataOffset   = 0x20;
	mEmbPalette             = nullptr;
	storeTIMG(img, (u8)'\0');
	DCFlushRange(mTexData, texBufferSize);
}

Texture::~Texture() {
    if (getCaptureFlag()) {
        delete[] mImage;
    }
    if (getEmbPaletteDelFlag()) {
        delete mEmbPalette;
    }
}

void Texture::storeTIMG(ResTIMG const *pTIMG, u8 param0) {
    GXTlut tlut;

    if (pTIMG && param0 < GX_BIGTLUT0) {
        mTexInfo = pTIMG;
        mTexData = (void *)((int)mTexInfo + mTexInfo->mImageDataOffset);

        if (mTexInfo->mImageDataOffset == 0) {
            mTexData = (void *)((int)mTexInfo + 0x20);
        }

        mpPalette = nullptr;
        mTlutName = 0;
        mWrapS = mTexInfo->mWrapS;
        mWrapT = mTexInfo->mWrapT;
        mMinFilter = mTexInfo->mMinFilterType;
        mMagFilter = mTexInfo->mMagFilterType;
        mMinLOD = mTexInfo->mMinLOD;
        mMaxLOD = mTexInfo->mMaxLOD;
        mLODBias = mTexInfo->mLODBias;

        if (mTexInfo->mPaletteEntryCount == 0) {
            initTexObj();
        } else {
            if (mTexInfo->mPaletteEntryCount > 0x100) {
                tlut = (GXTlut)((param0 % 4) + GX_BIGTLUT0);
            } else {
                tlut = (GXTlut)param0;
            }

            if (mEmbPalette == nullptr || !getEmbPaletteDelFlag()) {
                mEmbPalette = new Palette(
                    tlut, (GXTlutFmt)mTexInfo->mColorFormat, (JUTTransparency)mTexInfo->mAlphaEnabled,
                    mTexInfo->mPaletteEntryCount, (void *)(&mTexInfo->mTextureFormat + mTexInfo->mPaletteOffset)
                );
                mFlags = mFlags & 1 | 2;
            } else {
                mEmbPalette->storeTLUT(
                    tlut, (GXTlutFmt)mTexInfo->mColorFormat, (JUTTransparency)mTexInfo->mAlphaEnabled,
                    mTexInfo->mPaletteEntryCount, (void *)(&mTexInfo->mTextureFormat + mTexInfo->mPaletteOffset)
                );
            }
            attachPalette(mEmbPalette);
        }
    }
}

void Texture::attachPalette(Palette *param_0) {
    if (mTexInfo->mPaletteFormat) {
        if (param_0 == nullptr && mEmbPalette != nullptr) {
            mpPalette = mEmbPalette;
        } else {
            mpPalette = param_0;
        }
        GXTlut name = (GXTlut)mpPalette->getTlutName();
        initTexObj(name);
    }
}

void Texture::initTexObj() {
    GXBool mIsMIPmapEnabled;
    if (mTexInfo->mIsMIPmapEnabled != 0) {
        mIsMIPmapEnabled = 1;
    } else {
        mIsMIPmapEnabled = 0;
    }
    u8 *image = ((u8 *)mTexInfo);
    image += (mTexInfo->mImageDataOffset ? mTexInfo->mImageDataOffset : 0x20);
    GXInitTexObj(
        &mTexObj, image, mTexInfo->mSizeX, mTexInfo->mSizeY, (GXTexFmt)mTexInfo->mTextureFormat, (GXTexWrapMode)mWrapS,
        (GXTexWrapMode)mWrapT, mIsMIPmapEnabled
    );
    GXInitTexObjLOD(
        &mTexObj, (GXTexFilter)mMinFilter, (GXTexFilter)mMagFilter, mMinLOD / 8.0f, mMaxLOD / 8.0f, mLODBias / 100.0f,
        mTexInfo->mIsBiasClamp, mTexInfo->mDoEdgeLOD, (GXAnisotropy)mTexInfo->mIsMaxAnisotropy
    );
}

void Texture::initTexObj(GXTlut param_0) {
    GXBool mIsMIPmapEnabled;
    if (mTexInfo->mIsMIPmapEnabled != 0) {
        mIsMIPmapEnabled = 1;
    } else {
        mIsMIPmapEnabled = 0;
    }
    mTlutName = param_0;
    u8 *image = ((u8 *)mTexInfo);
    image += (mTexInfo->mImageDataOffset ? mTexInfo->mImageDataOffset : 0x20);
    GXInitTexObjCI(
        &mTexObj, image, mTexInfo->mSizeX, mTexInfo->mSizeY, (GXCITexFmt)mTexInfo->mTextureFormat, (GXTexWrapMode)mWrapS,
        (GXTexWrapMode)mWrapT, mIsMIPmapEnabled, param_0
    );
    GXInitTexObjLOD(
        &mTexObj, (GXTexFilter)mMinFilter, (GXTexFilter)mMagFilter, mMinLOD / 8.0f, mMaxLOD / 8.0f, mLODBias / 100.0f,
        mTexInfo->mIsBiasClamp, mTexInfo->mDoEdgeLOD, (GXAnisotropy)mTexInfo->mIsMaxAnisotropy
    );
}

void Texture::load(GXTexMapID param_0) {
    if (mpPalette) {
        mpPalette->load();
    }
    GXLoadTexObj(&mTexObj, param_0);
}

void Texture::capture(int x0, int y0, GXTexFmt textureFormat, bool useMIPmap, u8 doClear)
{
	if (!(mFlags & 1)) {
		return;
	}

	if (useMIPmap) {
		GXSetTexCopySrc(x0, y0, mTexInfo->mSizeX << 1, mTexInfo->mSizeY << 1);
	} else {
		GXSetTexCopySrc(x0, y0, mTexInfo->mSizeX, mTexInfo->mSizeY);
	}

	GXSetTexCopyDst(mTexInfo->mSizeX, mTexInfo->mSizeY, textureFormat, useMIPmap);
	GXCopyTex(mTexData, doClear);
	GXPixModeSync();
}

} // namespace EGG