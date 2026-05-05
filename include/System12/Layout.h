#ifndef _SYSTEM12_LAYOUT_H
#define _SYSTEM12_LAYOUT_H

#include <System12/FrameCounter.h>
#include <System12/WPTagProcessor.h>
#include <System12/WPWideTextWriter.h>
#include <egg/core/eggHeap.h>
#include <egg/core/eggMutex.h>
#include <egg/math/eggMatrix.h>
#include <egg/prim/eggList.h>
#include <nw4r/lyt.h>
#include <nw4r/ut.h>

#include <RevoSDK/gx.h>
#include <stl/wchar.h>

class System12TextBox : public nw4r::lyt::TextBox {
public:
	System12TextBox(const nw4r::lyt::res::TextBox* pRes, const nw4r::lyt::ResBlockSet& rBlockSet)
	    : nw4r::lyt::TextBox(pRes, rBlockSet)
	    , mWPTagProcessor(nullptr)
	{
		sysTextBoxInit();
	}

	virtual ~System12TextBox();                         // _08
	virtual u16 SetString(const wchar_t* pStr, u16 pos) // _7C
	{
		return SetString(pStr, pos, wcslen(pStr));
	}
	virtual u16 SetString(const wchar_t*, u16, u16);    // _80

	void sysTextBoxInit();
	u16 SetStringAdjust(const wchar_t*, u16);
	u16 SetStringAdjust(const wchar_t*, u16, u16);
	u16 setStringImpl(const wchar_t*, u16, u16);
	void adjustImpl();

	void SetTagProcessor(System12::WPTagProcessor* processor)
	{
		mWPTagProcessor = processor;
		nw4r::lyt::TextBox::SetTagProcessor(processor);
	}

	System12::WPTagProcessor* mWPTagProcessor; // _104

	static System12::WPTagProcessor sDefaultTagProcessor;
};

namespace System12 {

class AnimTransformDisposer : public EGG::Disposer {
public:
	AnimTransformDisposer(nw4r::lyt::Layout* layout, nw4r::lyt::AnimTransform* transform)
	    : mLayout(layout)
	    , mAnimTransform(transform)
	{
	}

	virtual ~AnimTransformDisposer(); // _08

	nw4r::lyt::Layout* mLayout;               // _10
	nw4r::lyt::AnimTransform* mAnimTransform; // _14
};

class System12LayoutImpl : public nw4r::lyt::Layout {
public:
	System12LayoutImpl() { }

	virtual ~System12LayoutImpl();                                                      // _08
	virtual bool Build(const void* pLytBinary, nw4r::lyt::ResourceAccessor* lytResBuf); // _0C

	nw4r::lyt::Pane* buildPane(s32 kind, const void* pBinary, const nw4r::lyt::ResBlockSet& rBlockSet);
};

class FontLessArcResourceAccessor : public nw4r::lyt::ArcResourceAccessor {
public:
	FontLessArcResourceAccessor() { }

	virtual ~FontLessArcResourceAccessor() { }                          // _08
	virtual void* GetResource(u32 type, const char* pName, u32* pSize); // _0C
};

class Layout {
public:
	class Animator : public FrameCounter {
	public:
		Animator() { }

		void setup(nw4r::lyt::AnimTransform*);

		nw4r::lyt::AnimTransform* _28;
	};

	Layout(EGG::Heap*);

	

	nw4r::lyt::AnimTransform* createAnimTransform(const char*, bool);
	Layout::Animator* createAnimator(nw4r::lyt::AnimTransform*);
	void registerAnimator(Animator*);
	nw4r::lyt::Pane* clonePane(const char*, bool);
	void clonePaneRecursiveImpl(nw4r::lyt::Pane*, nw4r::lyt::Pane*);
	nw4r::lyt::Pane* clonePaneImpl(const char*);
	bool buildResBlock(const void*, nw4r::lyt::ResourceAccessor*, nw4r::lyt::ResBlockSet*);
	void bind(const char*, bool, bool);
	void bind(const char*, nw4r::lyt::Pane*, bool, bool);
	void calc();
	void calculateMtx();
	void calculateMtx(const EGG::Matrix34f&);
	void draw(_GXCullMode, bool);
	void draw(const EGG::Matrix34f&, _GXCullMode, bool);
	void allocatorAssert();

	System12LayoutImpl* getLayout() const { return mLayout; }
	nw4r::lyt::Pane* getRootPane() const { return mLayout->GetRootPane(); }

	int _00; 							                  // _00
	int _04;							                  // _04
	int _08; 							                  // _08
	int _0C; 							                  // _0C
	int _10; 							                  // _10

    virtual void construct(const char*, const char*, bool);

	void* mLytResource;                                       // _18
	System12LayoutImpl* mLayout;                              // _1C
	FontLessArcResourceAccessor mFontLessArcResourceAccessor; // _20
	nw4r::lyt::ResBlockSet _D0;                               // _D0
	u8* mArcDataPtr;                                          // _E0
	EGG::Heap* mHeap;                                         // _E4

	static EGG::Mutex sMutex;
};

} // namespace System12

#endif
