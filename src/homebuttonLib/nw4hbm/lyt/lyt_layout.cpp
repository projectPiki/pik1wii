#include "homebuttonLib/nw4hbm/lyt/layout.h"
#include "homebuttonLib/nw4hbm/lyt/animation.h"
#include "homebuttonLib/nw4hbm/lyt/bounding.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/group.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/lyt/picture.h"
#include "homebuttonLib/nw4hbm/lyt/textBox.h"
#include "homebuttonLib/nw4hbm/lyt/types.h"
#include "homebuttonLib/nw4hbm/lyt/window.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include "homebuttonLib/nw4hbm/ut/Rect.h"
#include "homebuttonLib/nw4hbm/ut/RuntimeTypeInfo.h"
#include <types.h>
#include <new.h>

namespace {

	using namespace nw4hbm;
	using namespace nw4hbm::lyt;


// Wait until these guys hear about C++11
template <class T>
T *CreateObject()
{
	void *pMem = Layout::AllocMemory(sizeof(T));

	if (pMem)
		return new (pMem) T;
	else
		return nullptr;
}

template <class T, typename P1>
T *CreateObject(P1 p1)
{
	void *pMem = Layout::AllocMemory(sizeof(T));

	if (pMem)
		return new (pMem) T(p1);
	else
		return nullptr;
}

template <class T, typename P1, typename P2>
T *CreateObject(P1 p1, P2 p2)
{
	void *pMem = Layout::AllocMemory(sizeof(T));

	if (pMem)
		return new (pMem) T(p1, p2);
	else
		return nullptr;
}

void SetTagProcessorImpl(Pane *pPane,
                         ut::TagProcessorBase<wchar_t> *pTagProcessor)
{
	if (TextBox *pTextBox = ut::DynamicCast<TextBox *>(pPane))
		pTextBox->SetTagProcessor(pTagProcessor);

	NW4HBM_RANGE_FOR(it, pPane->GetChildList())
		SetTagProcessorImpl(&(*it), pTagProcessor);
}

} // unnamed namespace

namespace nw4hbm { 
namespace lyt {

MEMAllocator *Layout::mspAllocator;

Layout::Layout()
	: mpRootPane(nullptr)
	, mpGroupContainer(nullptr)
	, mLayoutSize(0.0f, 0.0f)
	, mOriginType(OriginType_TopLeft)
	{
	}

Layout::~Layout()
{
	if (mpGroupContainer)
	{
		mpGroupContainer->~GroupContainer();
		FreeMemory(mpGroupContainer);
	}

	if (mpRootPane && !mpRootPane->IsUserAllocated())
	{
		mpRootPane->~Pane();
		FreeMemory(mpRootPane);
	}

	NW4HBM_RANGE_FOR_NO_AUTO_INC(it, mAnimTransList)
	{
		DECLTYPE(it) currIt = it++;

		mAnimTransList.Erase(currIt);
		currIt->~AnimTransform();
		FreeMemory(&(*currIt));
	}
}

bool Layout::Build(void const *lytResBuf, ResourceAccessor *pResAcsr)
{
	res::BinaryFileHeader const *fileHead =
		static_cast<res::BinaryFileHeader const *>(lytResBuf);

	if (!detail::TestFileHeader(*fileHead, res::SIGNATURE_LAYOUT))
		return false;

	ResBlockSet resBlockSet = {};
	resBlockSet.pResAccessor = pResAcsr;

	Pane *pParentPane = nullptr;
	Pane *pLastPane = nullptr;
	bool bReadRootGroup = false;
	int groupNestLevel = 0;
	void const *dataPtr = POINTER_ADD(lytResBuf, fileHead->headerSize);

	for (int i = 0; i < fileHead->dataBlocks; ++i)
	{
		res::DataBlockHeader const *pDataBlockHead =
			static_cast<res::DataBlockHeader const *>(dataPtr);

		switch (detail::GetSignatureInt(pDataBlockHead->kind))
		{
		case res::SIGNATURE_LAYOUT_BLOCK:
		{
			res::Layout const *pResLyt =
				static_cast<res::Layout const *>(dataPtr);

			mOriginType = static_cast<OriginType>(pResLyt->originType != 0);
			mLayoutSize = pResLyt->layoutSize;
		}
			break;

		case res::SIGNATURE_TEXTURE_LIST_BLOCK:
			resBlockSet.pTextureList =
				static_cast<res::TextureList const *>(dataPtr);
			break;

		case res::SIGNATURE_FONT_LIST_BLOCK:
			resBlockSet.pFontList = static_cast<res::FontList const *>(dataPtr);
			break;

		case res::SIGNATURE_MATERIAL_LIST_BLOCK:
			resBlockSet.pMaterialList =
				static_cast<res::MaterialList const *>(dataPtr);
			break;

		case res::SIGNATURE_PANE_BLOCK:
		case res::SIGNATURE_BOUNDING_BLOCK:
		case res::SIGNATURE_PICTURE_BLOCK:
		case res::SIGNATURE_TEXT_BOX_BLOCK:
		case res::SIGNATURE_WINDOW_BLOCK:
			if (Pane *pPane =
			        BuildPaneObj(detail::GetSignatureInt(pDataBlockHead->kind),
			                     dataPtr, resBlockSet))
			{
				if (!mpRootPane)
					mpRootPane = pPane;

				if (pParentPane)
					pParentPane->AppendChild(pPane);

				pLastPane = pPane;
			}

			break;

		case res::SIGNATURE_PANE_START_BLOCK:
			pParentPane = pLastPane;
			break;

		case res::SIGNATURE_PANE_END_BLOCK:
			pLastPane = pParentPane;
			pParentPane = pLastPane->GetParent();
			break;

		case res::SIGNATURE_GROUP_BLOCK:
			if (!bReadRootGroup)
			{
				bReadRootGroup = true;
				mpGroupContainer = CreateObject<GroupContainer>();
			}
			else if (mpGroupContainer && groupNestLevel == 1)
			{
				if (Group *pGroup = CreateObject<Group>(
						reinterpret_cast<res::Group const *>(pDataBlockHead),
						mpRootPane))
				{
					mpGroupContainer->AppendGroup(pGroup);
				}
			}

			break;

		case res::SIGNATURE_GROUP_BLOCK_START:
			++groupNestLevel;
			break;

		case res::SIGNATURE_GROUP_BLOCK_END:
			--groupNestLevel;
			break;
		}

		dataPtr = POINTER_ADD(dataPtr, pDataBlockHead->size);
	}

	return true;
}

AnimTransform *Layout::CreateAnimTransform(void const *anmResBuf,
                                           ResourceAccessor *pResAcsr)
{
	res::BinaryFileHeader const *pFileHead =
		static_cast<res::BinaryFileHeader const *>(anmResBuf);

	if (!detail::TestFileHeader(*pFileHead))
		return nullptr;

	res::AnimationBlock const *pInfoBlock = nullptr;
	res::DataBlockHeader const *pDataBlockHead =
		detail::ConvertOffsToPtr<res::DataBlockHeader>(pFileHead,
	                                                   pFileHead->headerSize);

	AnimTransform *ret = nullptr;

	for (int i = 0; i < pFileHead->dataBlocks; ++i)
	{
		switch (detail::GetSignatureInt(pDataBlockHead->kind))
		{
		case res::SIGNATURE_ANIM_INFO_BLOCK:
			switch (detail::GetSignatureInt(pFileHead->signature))
			{
			case res::SIGNATURE_ANIMATION:
			case res::AnimationInfo::SIGNATURE_PANE_PAIN_SRT_INFO:
			case res::AnimationInfo::SIGNATURE_PANE_VISIBILITY_INFO:
			case res::AnimationInfo::SIGNATURE_PANE_VERTEX_COLOR_INFO:
			case res::AnimationInfo::SIGNATURE_MATERIAL_COLOR_INFO:
			case res::AnimationInfo::SIGNATURE_MATERIAL_TEXTURE_SRT_INFO:
			case res::AnimationInfo::SIGNATURE_MATERIAL_TEXTURE_PATTERN_INFO:
				if (AnimTransformBasic *pAnimTrans =
				        CreateObject<AnimTransformBasic>())
				{
					pInfoBlock = reinterpret_cast<res::AnimationBlock const *>(
						pDataBlockHead);

					pAnimTrans->SetResource(pInfoBlock, pResAcsr);
					ret = pAnimTrans;
				}

				break;
			}

			if (ret)
				mAnimTransList.PushBack(ret);

			break;
		}

		pDataBlockHead = detail::ConvertOffsToPtr<res::DataBlockHeader>(
			pDataBlockHead, pDataBlockHead->size);
	}

	return ret;
}

void Layout::BindAnimation(AnimTransform *pAnimTrans)
{
	if (mpRootPane)
		mpRootPane->BindAnimation(pAnimTrans, true);
}

void Layout::UnbindAnimation(AnimTransform *pAnimTrans)
{
	if (mpRootPane)
		mpRootPane->UnbindAnimation(pAnimTrans, true);
}

void Layout::UnbindAllAnimation()
{
	UnbindAnimation(nullptr);
}

void Layout::SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable)
{
	if (mpRootPane)
		mpRootPane->SetAnimationEnable(pAnimTrans, bEnable, true);
}

void Layout::CalculateMtx(DrawInfo const &drawInfo)
{
	if (mpRootPane)
		mpRootPane->CalculateMtx(drawInfo);
}

void Layout::Draw(DrawInfo const &drawInfo)
{
	if (mpRootPane)
		mpRootPane->Draw(drawInfo);
}

void Layout::Animate(u32 option)
{
	if (mpRootPane)
		mpRootPane->Animate(option);
}

ut::Rect Layout::GetLayoutRect() const
{
	if (mOriginType == OriginType_Center)
	{
		return ut::Rect(-mLayoutSize.width / 2.0f, mLayoutSize.height / 2.0f,
		                mLayoutSize.width / 2.0f, -mLayoutSize.height / 2.0f);
	}
	else /* if (mOriginType == OriginType_TopLeft) */
	{
		return ut::Rect(0.0f, 0.0f, mLayoutSize.width, mLayoutSize.height);
	}
}

void Layout::SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor)
{
	SetTagProcessorImpl(mpRootPane, pTagProcessor);
}

Pane *Layout::BuildPaneObj(s32 kind, void const *dataPtr,
                           ResBlockSet const &resBlockSet)
{
	switch (kind)
	{
	case res::SIGNATURE_PANE_BLOCK:
	{
		res::Pane const *pResPane = static_cast<res::Pane const *>(dataPtr);
		(void)0;

		return CreateObject<Pane>(pResPane);
	}

	case res::SIGNATURE_PICTURE_BLOCK:
	{
		res::Picture const *pResPic =
			static_cast<res::Picture const *>(dataPtr);
		(void)0;

		return CreateObject<Picture>(pResPic, resBlockSet);
	}

	case res::SIGNATURE_TEXT_BOX_BLOCK:
	{
		res::TextBox const *pBlock = static_cast<res::TextBox const *>(dataPtr);

		return CreateObject<TextBox>(pBlock, resBlockSet);
	}

	case res::SIGNATURE_WINDOW_BLOCK:
	{
		res::Window const *pBlock = static_cast<res::Window const *>(dataPtr);

		return CreateObject<Window>(pBlock, resBlockSet);
	}

	case res::SIGNATURE_BOUNDING_BLOCK:
	{
		res::Bounding const *pResBounding =
			static_cast<res::Bounding const *>(dataPtr);
		(void)0;

		return CreateObject<Bounding>(pResBounding, resBlockSet);
	}

	default:
		(void)0;

		return nullptr;
	}
}

}} // namespace nw4hbm::lyt
