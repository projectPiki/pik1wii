#include "homebuttonLib/nw4hbm/lyt/window.h"
#include "homebuttonLib/nw4hbm/lyt/animation.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/layout.h"
#include "homebuttonLib/nw4hbm/lyt/material.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/lyt/types.h"
#include "homebuttonLib/nw4hbm/math/types.h"
#include "homebuttonLib/nw4hbm/ut/Color.h"
#include "homebuttonLib/nw4hbm/ut/inlines.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include "homebuttonLib/nw4hbm/ut/RuntimeTypeInfo.h"
#include <types.h>
#include <new.h>


struct TextureFlipInfo {
	u8 coords[4][2];	// _00
	u8 idx[2];			// _08
};

namespace
{
	using namespace nw4hbm;
	using namespace nw4hbm::lyt;

	// NOTE the misspelling of GetTextureFlipInfo
	TextureFlipInfo const &GetTexutreFlipInfo(u8 textureFlip);

	void GetLTFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
	                    Size const &winSize, WindowFrameSize const &frameSize);
	void GetLTTexCoord(math::VEC2 *texCds, Size const &polSize,
	                   Size const &texSize, u8 textureFlip);
	void GetRTFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
	                    Size const &winSize, WindowFrameSize const &frameSize);
	void GetRTTexCoord(math::VEC2 *texCds, Size const &polSize,
	                   Size const &texSize, u8 textureFlip);
	void GetLBFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
	                    Size const &winSize, WindowFrameSize const &frameSize);
	void GetLBTexCoord(math::VEC2 *texCds, Size const &polSize,
	                   Size const &texSize, u8 textureFlip);
	void GetRBFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
	                    Size const &winSize, WindowFrameSize const &frameSize);
	void GetRBTexCoord(math::VEC2 *texCds, Size const &polSize,
	                   Size const &texSize, u8 textureFlip);
} // namespace

namespace nw4hbm { 
namespace lyt {
	
	ut::detail::RuntimeTypeInfo const Window::typeInfo(&Pane::typeInfo);
	
} // namespace lyt
} // namespace nw4hbm

// TODO: clean up

namespace {

TextureFlipInfo const &GetTexutreFlipInfo(u8 textureFlip)
{
	// clang-format off

	static TextureFlipInfo flipInfos[] =            // 0   1   2   3
	{                                   // in order of LT  RT  LB  RB
		{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {0, 1}}, // 0   1   2   3             no flip
		{{{1, 0}, {0, 0}, {1, 1}, {0, 1}}, {0, 1}}, // 1   0   3   2             horizontal flip
		{{{0, 1}, {1, 1}, {0, 0}, {1, 0}}, {0, 1}}, // 2   3   0   1             vertical flip
		{{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, {1, 0}}, // 2   0   3   1, index flip cw  90 deg
		{{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, {0, 1}}, // 3   2   1   0             cw 180 deg
		{{{1, 0}, {1, 1}, {0, 0}, {0, 1}}, {1, 0}}  // 1   3   0   2, index flip cw 270 deg (ccw 90 deg)
	};

	// clang-format on

	return flipInfos[textureFlip];
}

void GetLTFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
                    Size const &winSize, WindowFrameSize const &frameSize)
{
	*pPt = basePt;

	pSize->width = winSize.width - frameSize.r;
	pSize->height = frameSize.t;
}

void GetLTTexCoord(math::VEC2 *texCds, Size const &polSize, Size const &texSize,
                   u8 textureFlip)
{
	TextureFlipInfo const &flipInfo = GetTexutreFlipInfo(textureFlip);
	int ix = flipInfo.idx[0];
	int iy = flipInfo.idx[1];
	math::VEC2 const tSz(texSize.width, texSize.height);

	texCds[0][ix] = texCds[2][ix] = flipInfo.coords[0][ix];
	texCds[0][iy] = texCds[1][iy] = flipInfo.coords[0][iy];

	texCds[3][ix] = texCds[1][ix] =
		flipInfo.coords[0][ix]
		+ polSize.width
			  / (tSz[ix] * (flipInfo.coords[1][ix] - flipInfo.coords[0][ix]));

	texCds[3][iy] = texCds[2][iy] =
		flipInfo.coords[0][iy]
		+ polSize.height
			  / (tSz[iy] * (flipInfo.coords[2][iy] - flipInfo.coords[0][iy]));
}

void GetRTFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
                    Size const &winSize, WindowFrameSize const &frameSize)
{
	*pPt = math::VEC2(basePt.x + winSize.width - frameSize.r, basePt.y);

	pSize->width = frameSize.r;
	pSize->height = winSize.height - frameSize.b;
}

void GetRTTexCoord(math::VEC2 *texCds, Size const &polSize, Size const &texSize,
                   u8 textureFlip)
{
	TextureFlipInfo const &flipInfo = GetTexutreFlipInfo(textureFlip);
	int ix = flipInfo.idx[0];
	int iy = flipInfo.idx[1];
	math::VEC2 const tSz(texSize.width, texSize.height);

	texCds[1][ix] = texCds[3][ix] = flipInfo.coords[1][ix];
	texCds[1][iy] = texCds[0][iy] = flipInfo.coords[1][iy];

	texCds[2][ix] = texCds[0][ix] =
		flipInfo.coords[1][ix]
		+ polSize.width
			  / (tSz[ix] * (flipInfo.coords[0][ix] - flipInfo.coords[1][ix]));

	texCds[2][iy] = texCds[3][iy] =
		flipInfo.coords[1][iy]
		+ polSize.height
			  / (tSz[iy] * (flipInfo.coords[3][iy] - flipInfo.coords[1][iy]));
}

void GetLBFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
                    Size const &winSize, WindowFrameSize const &frameSize)
{
	*pPt = math::VEC2(basePt.x, basePt.y + frameSize.t);

	pSize->width = frameSize.l;
	pSize->height = winSize.height - frameSize.t;
}

void GetLBTexCoord(math::VEC2 *texCds, Size const &polSize, Size const &texSize,
                   u8 textureFlip)
{
	TextureFlipInfo const &flipInfo = GetTexutreFlipInfo(textureFlip);
	int ix = flipInfo.idx[0];
	int iy = flipInfo.idx[1];
	math::VEC2 const tSz(texSize.width, texSize.height);

	texCds[2][ix] = texCds[0][ix] = flipInfo.coords[2][ix];
	texCds[2][iy] = texCds[3][iy] = flipInfo.coords[2][iy];

	texCds[1][ix] = texCds[3][ix] =
		flipInfo.coords[2][ix]
		+ polSize.width
			  / (tSz[ix] * (flipInfo.coords[3][ix] - flipInfo.coords[2][ix]));

	texCds[1][iy] = texCds[0][iy] =
		flipInfo.coords[2][iy]
		+ polSize.height
			  / (tSz[iy] * (flipInfo.coords[0][iy] - flipInfo.coords[2][iy]));
}

void GetRBFrameSize(math::VEC2 *pPt, Size *pSize, math::VEC2 const &basePt,
                    Size const &winSize, WindowFrameSize const &frameSize)
{
	*pPt = math::VEC2(basePt.x + frameSize.l,
	                  basePt.y + winSize.height - frameSize.b);

	pSize->width = winSize.width - frameSize.l;
	pSize->height = frameSize.b;
}

void GetRBTexCoord(math::VEC2 *texCds, Size const &polSize, Size const &texSize,
                   u8 textureFlip)
{
	TextureFlipInfo const &flipInfo = GetTexutreFlipInfo(textureFlip);
	int ix = flipInfo.idx[0];
	int iy = flipInfo.idx[1];
	math::VEC2 const tSz(texSize.width, texSize.height);

	texCds[3][ix] = texCds[1][ix] = flipInfo.coords[3][ix];
	texCds[3][iy] = texCds[2][iy] = flipInfo.coords[3][iy];

	texCds[0][ix] = texCds[2][ix] =
		flipInfo.coords[3][ix]
		+ polSize.width
			  / (tSz[ix] * (flipInfo.coords[2][ix] - flipInfo.coords[3][ix]));

	texCds[0][iy] = texCds[1][iy] =
		flipInfo.coords[3][iy]
		+ polSize.height
			  / (tSz[iy] * (flipInfo.coords[1][iy] - flipInfo.coords[3][iy]));
}

} // namespace

namespace nw4hbm { 
namespace lyt {

Window::Window(res::Window const *pBlock, ResBlockSet const &resBlockSet):
	Pane(pBlock)
{
	mContentInflation = pBlock->inflation;

	u32 const *matOffsTbl = detail::ConvertOffsToPtr<u32>(
		resBlockSet.pMaterialList, sizeof *resBlockSet.pMaterialList);

	{ // 0x28a3 wants lexical_block
		res::WindowContent const *pResContent =
			detail::ConvertOffsToPtr<res::WindowContent>(pBlock,
		                                                 pBlock->contentOffset);

		for (int i = 0; i < (int)ARRAY_SIZE(mContent.vtxColors); ++i)
			mContent.vtxColors[i] = pResContent->vtxCols[i];

		if (pResContent->texCoordNum)
		{
			u8 texCoordNum = ut::Min<u8>(pResContent->texCoordNum, 8);
			mContent.texCoordAry.Reserve(texCoordNum);

			if (!mContent.texCoordAry.IsEmpty())
				mContent.texCoordAry.Copy(pResContent + 1, texCoordNum);
		}

		if (void *pMemMaterial = Layout::AllocMemory(sizeof(Material)))
		{
			res::Material const *pResMaterial =
				detail::ConvertOffsToPtr<res::Material>(
					resBlockSet.pMaterialList,
					matOffsTbl[pResContent->materialIdx]);

			mpMaterial = new (pMemMaterial) Material(pResMaterial, resBlockSet);
		}
	}

	mFrameNum	= 0;
	mFrames		= nullptr;

	if (pBlock->frameNum)
	{
		if ((mFrames = static_cast<Frame *>(
				 Layout::AllocMemory(sizeof *mFrames * pBlock->frameNum))))
		{
			mFrameNum = pBlock->frameNum;

			u32 const *frameOffsetTable = detail::ConvertOffsToPtr<u32>(
				pBlock, pBlock->frameOffsetTableOffset);

			for (int i = 0; i < mFrameNum; ++i)
			{
				res::WindowFrame const *pResWindowFrame =
					detail::ConvertOffsToPtr<res::WindowFrame>(
						pBlock, frameOffsetTable[i]);

				mFrames[i].textureFlip	= pResWindowFrame->textureFlip;
				mFrames[i].pMaterial	= nullptr;

				if (void *pMemMaterial = Layout::AllocMemory(sizeof(Material)))
				{
					res::Material const *pResMaterial =
						detail::ConvertOffsToPtr<res::Material>(
							resBlockSet.pMaterialList,
							matOffsTbl[pResWindowFrame->materialIdx]);

					mFrames[i].pMaterial =
						new (pMemMaterial) Material(pResMaterial, resBlockSet);
				}
			}
		}
	}
}

Window::~Window()
{
	if (mFrames)
	{
		for (int i = 0; i < mFrameNum; ++i)
		{
			mFrames[i].pMaterial->~Material();
			Layout::FreeMemory(mFrames[i].pMaterial);
		}

		Layout::FreeMemory(mFrames);
	}

	if (mpMaterial && !mpMaterial->IsUserAllocated())
	{
		mpMaterial->~Material();
		Layout::FreeMemory(mpMaterial);
		mpMaterial = nullptr;
	}

	mContent.texCoordAry.Free();
}

Material *Window::FindMaterialByName(char const *findName, bool bRecursive)
{
	if (mpMaterial
	    && detail::EqualsMaterialName(mpMaterial->GetName(), findName))
	{
		return mpMaterial;
	}

	for (int i = 0; i < mFrameNum; ++i)
	{
		if (detail::EqualsMaterialName(mFrames[i].pMaterial->GetName(),
		                               findName))
		{
			return mFrames[i].pMaterial;
		}
	}

	if (bRecursive)
	{
		NW4HBM_RANGE_FOR(it, mChildList)
		{
			if (Material *pMat = it->FindMaterialByName(findName, true))
				return pMat;
		}
	}

	return nullptr;
}

AnimationLink *Window::FindAnimationLink(AnimTransform *pAnimTrans)
{
	if (AnimationLink *ret = Pane::FindAnimationLink(pAnimTrans))
		return ret;

	for (int i = 0; i < mFrameNum; ++i)
	{
		if (AnimationLink *ret =
		        mFrames[i].pMaterial->FindAnimationLink(pAnimTrans))
		{
			return ret;
		}
	}

	return nullptr;
}

void Window::SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable,
                                bool bRecursive)
{
	for (int i = 0; i < mFrameNum; ++i)
		mFrames[i].pMaterial->SetAnimationEnable(pAnimTrans, bEnable);

	Pane::SetAnimationEnable(pAnimTrans, bEnable, bRecursive);
}

ut::Color Window::GetVtxColor(u32 idx) const
{
	return mContent.vtxColors[idx];
}

void Window::SetVtxColor(u32 idx, ut::Color value)
{
	mContent.vtxColors[idx] = value;
}

u8 Window::GetVtxColorElement(u32 idx) const
{
	return detail::GetVtxColorElement(mContent.vtxColors, idx);
}

void Window::SetVtxColorElement(u32 idx, u8 value)
{
	detail::SetVtxColorElement(mContent.vtxColors, idx, value);
}

void Window::DrawSelf(DrawInfo const &drawInfo)
{
	LoadMtx(drawInfo);

	WindowFrameSize frameSize = GetFrameSize(mFrameNum, mFrames);
	math::VEC2 basePt = GetVtxPos();

	DrawContent(basePt, frameSize, mGlbAlpha);

	switch (mFrameNum)
	{
	case 1:
		DrawFrame(basePt, *mFrames, frameSize, mGlbAlpha);
		break;

	case 4:
		DrawFrame4(basePt, mFrames, frameSize, mGlbAlpha);
		break;

	case 8:
		DrawFrame8(basePt, mFrames, frameSize, mGlbAlpha);
		break;
	}
}

void Window::AnimateSelf(u32 option)
{
	Pane::AnimateSelf(option);

	if (detail::TestBit(mFlag, 0) || !(option & 1))
	{
		for (int i = 0; i < mFrameNum; ++i)
			mFrames[i].pMaterial->Animate();
	}
}

void Window::UnbindAnimationSelf(AnimTransform *pAnimTrans)
{
	for (int i = 0; i < mFrameNum; ++i)
		mFrames[i].pMaterial->UnbindAnimation(pAnimTrans);

	Pane::UnbindAnimationSelf(pAnimTrans);
}

void Window::DrawContent(math::VEC2 const &basePt,
                         WindowFrameSize const &frameSize, u8 alpha)
{
	bool bUseVtxCol = mpMaterial->SetupGX(
		detail::IsModulateVertexColor(mContent.vtxColors, alpha), alpha);

	detail::SetVertexFormat(bUseVtxCol, mContent.texCoordAry.GetSize());

	detail::DrawQuad(math::VEC2(basePt.x + frameSize.l - mContentInflation.l,
	                            basePt.y + frameSize.t - mContentInflation.t),
	                 Size(mSize.width - frameSize.l + mContentInflation.l
	                          - frameSize.r + mContentInflation.r,
	                      mSize.height - frameSize.t + mContentInflation.t
	                          - frameSize.b + mContentInflation.b),
	                 mContent.texCoordAry.GetSize(),
	                 mContent.texCoordAry.GetArray(),
	                 bUseVtxCol ? mContent.vtxColors : nullptr, alpha);
}

void Window::DrawFrame(math::VEC2 const &basePt, Frame const &frame,
                       WindowFrameSize const &frameSize, u8 alpha)
{
	u8 const texCoordNum = 1;

	bool bUseVtxCol = frame.pMaterial->SetupGX(
		detail::IsModulateVertexColor(nullptr, alpha), alpha);
	detail::SetVertexFormat(bUseVtxCol, texCoordNum);

	Size texSize = detail::GetTextureSize(frame.pMaterial, 0);
	ut::Color vtxColors[4];
	detail::TexCoords texCds[1];
	math::VEC2 polPt;
	Size polSize;

#define DrawFrame1Quad_(corner_, frameIdx_)									\
	do																		\
	{																		\
		Get ## corner_ ## FrameSize(&polPt, &polSize, basePt, mSize,		\
		                            frameSize);								\
		Get ## corner_ ## TexCoord(*texCds, polSize, texSize, frameIdx_);	\
																			\
		detail::DrawQuad(polPt, polSize, texCoordNum, texCds,				\
		                 bUseVtxCol ? vtxColors : nullptr, alpha);			\
	} while (0)

	DrawFrame1Quad_(LT, 0);
	DrawFrame1Quad_(RT, 1);
	DrawFrame1Quad_(RB, 4);
	DrawFrame1Quad_(LB, 2);

#undef DrawFrame1Quad_
}

void Window::DrawFrame4(math::VEC2 const &basePt, Frame const *frames,
                        WindowFrameSize const &frameSize, u8 alpha)
{
	u8 const texCoordNum = 1;

	ut::Color vtxColors[4];
	detail::TexCoords texCds[1];
	math::VEC2 polPt;
	Size polSize;
	bool bModVtxCol = detail::IsModulateVertexColor(nullptr, alpha);

#define DrawFrame4Quad_(corner_, frameIdx_)								\
	do																	\
	{																	\
		bool bUseVtxCol =												\
			frames[frameIdx_].pMaterial->SetupGX(bModVtxCol, alpha);	\
																		\
		Get ## corner_ ## FrameSize(&polPt, &polSize, basePt, mSize,	\
		                            frameSize);							\
		Get ## corner_ ## TexCoord(										\
			*texCds, polSize,											\
			detail::GetTextureSize(frames[frameIdx_].pMaterial, 0),		\
			frames[frameIdx_].textureFlip);								\
																		\
		detail::SetVertexFormat(bUseVtxCol, texCoordNum);				\
																		\
		detail::DrawQuad(polPt, polSize, texCoordNum, texCds,			\
		                 bUseVtxCol ? vtxColors : nullptr, alpha);		\
	} while (0)

	DrawFrame4Quad_(LT, 0);
	DrawFrame4Quad_(RT, 1);
	DrawFrame4Quad_(RB, 3);
	DrawFrame4Quad_(LB, 2);

#undef DrawFrame4Quad_
}

void Window::DrawFrame8(math::VEC2 const &basePt, Frame const *frames,
                        WindowFrameSize const &frameSize, u8 alpha)
{
	u8 const texCoordNum = 1;

	ut::Color vtxColors[4];
	detail::TexCoords texCds[1];
	Size polSize;
	bool bModVtxCol = detail::IsModulateVertexColor(nullptr, alpha);

#define DrawFrame8Quad_(corner_, frameIdx_, polSizeInit_, basePtInit_)		\
	do																		\
	{																		\
		bool bUseVtxCol =													\
			frames[frameIdx_].pMaterial->SetupGX(bModVtxCol, alpha);		\
		polSize = Size polSizeInit_;										\
																			\
		Get ## corner_ ## TexCoord(											\
			*texCds, polSize,												\
			detail::GetTextureSize(frames[frameIdx_].pMaterial, 0),			\
			frames[frameIdx_].textureFlip);									\
																			\
		detail::SetVertexFormat(bUseVtxCol, texCoordNum);					\
																			\
		detail::DrawQuad(DF8Q_CTOR_ basePtInit_, polSize, texCoordNum,		\
		                 texCds, bUseVtxCol ? vtxColors : nullptr, alpha);	\
	} while (0)

#define DF8Q_CTOR_ // avoid copy construction specifically for this first call
	DrawFrame8Quad_(LT, 0, (frameSize.l, frameSize.t), basePt);
#undef DF8Q_CTOR_

#define DF8Q_CTOR_ math::VEC2

	DrawFrame8Quad_(LT, 6,
		(mSize.width - frameSize.l - frameSize.r, frameSize.t),
		(basePt.x + frameSize.l, basePt.y)
	);
	DrawFrame8Quad_(RT, 1,
		(frameSize.r, frameSize.t),
		(basePt.x + mSize.width - frameSize.r, basePt.y)
	);
	DrawFrame8Quad_(RT, 5,
		(frameSize.r, mSize.height - frameSize.t - frameSize.b),
		(basePt.x + mSize.width - frameSize.r, basePt.y + frameSize.t)
	);
	DrawFrame8Quad_(RB, 3, (frameSize.r, frameSize.b),
		(basePt.x + mSize.width - frameSize.r,
		 basePt.y + mSize.height - frameSize.b)
	);
	DrawFrame8Quad_(RB, 7,
		(mSize.width - frameSize.l - frameSize.r, frameSize.b),
		(basePt.x + frameSize.l, basePt.y + mSize.height - frameSize.b)
	);
	DrawFrame8Quad_(LB, 2,
		(frameSize.l, frameSize.b),
		(basePt.x, basePt.y + mSize.height - frameSize.b)
	);
	DrawFrame8Quad_(LB, 4,
		(frameSize.l, mSize.height - frameSize.t - frameSize.b),
		(basePt.x, basePt.y + frameSize.t)
	);

#undef DF8Q_CTOR_
#undef DrawFrame8Quad_
}

WindowFrameSize Window::GetFrameSize(u8 frameNum, Frame const *frames)
{
	WindowFrameSize ret = {};

	switch (frameNum)
	{
	case 1:
	{
		Size texSize = detail::GetTextureSize(frames->pMaterial, 0);
		ret.l = texSize.width;
		ret.t = texSize.height;

		ret.r = texSize.width;
		ret.b = texSize.height;
	}
		break;

	case 4:
	case 8:
	{
		Size texSize = detail::GetTextureSize(frames[0].pMaterial, 0);
		ret.l = texSize.width;
		ret.t = texSize.height;

		texSize = detail::GetTextureSize(frames[3].pMaterial, 0);
		ret.r = texSize.width;
		ret.b = texSize.height;
	}
		break;
	}

	return ret;
}

Material *Window::GetFrameMaterial(u32 frameIdx) const
{
	if (frameIdx >= mFrameNum)
		return nullptr;

	return mFrames[frameIdx].pMaterial;
}

Material *Window::GetContentMaterial() const
{
	return GetMaterial();
}

} // namespace lyt
} // namespace nw4hbm
