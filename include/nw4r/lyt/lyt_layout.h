#ifndef NW4R_LYT_LAYOUT_H
#define NW4R_LYT_LAYOUT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_animation.h>
#include <nw4r/lyt/lyt_types.h>

#include <nw4r/ut.h>
#include <stl/new.h>
#include <RevoSDK/MEM.h>

namespace nw4r {
namespace lyt {

// Forward declarations
class ArcResourceAccessor;
class DrawInfo;
class GroupContainer;
class Pane;

/******************************************************************************
 *
 * OriginType
 *
 ******************************************************************************/
enum OriginType {
    ORIGINTYPE_TOPLEFT,
    ORIGINTYPE_CENTER,

    ORIGINTYPE_MAX
};

namespace res {

/******************************************************************************
 *
 * LYT1 binary layout
 *
 ******************************************************************************/
struct Layout {
    static const u32 SIGNATURE = FOURCC('l', 'y', 't', '1');

    DataBlockHeader blockHeader; // _00
    u8 originType;               // _08
    u8 PADDING_0x9[0xC - 0x9];   // _09
    Size layoutSize;             // _0C
};

} // namespace res

/******************************************************************************
 *
 * Layout
 *
 ******************************************************************************/
class Layout {
public:
    static const u32 SIGNATURE = FOURCC('R', 'L', 'Y', 'T');
    static const u32 SIGNATURE_ANIMATION = FOURCC('R', 'L', 'A', 'N');

public:
    Layout();
	
    virtual ~Layout(); 																					     // _08
    virtual bool Build(const void* pLytBinary, ResourceAccessor* pAccessor);							     // _0C
    virtual AnimTransform *CreateAnimTransform();															 // _10
    virtual AnimTransform *CreateAnimTransform(const void *animResBuf, ResourceAccessor *pResAcsr);		     // _14
    virtual AnimTransform *CreateAnimTransform(const AnimResource &animRes, ResourceAccessor *pResAcsr);	 // _18
    virtual void BindAnimation(AnimTransform* pAnimTrans);  											     // _1C
    virtual void UnbindAnimation(AnimTransform* pAnimTrans); 											     // _20
    virtual void UnbindAllAnimation();															             // _24
	virtual bool BindAnimationAuto(const AnimResource &animRes, ResourceAccessor *pResAcsr); 			     // _28
    virtual void SetAnimationEnable(AnimTransform* pAnimTrans,  bool enable);							     // _2C
    virtual void CalculateMtx(const DrawInfo& rInfo); 													     // _30
    virtual void Draw(const DrawInfo& rInfo);          													     // _34
    virtual void Animate(u32 option);              													    	 // _38
    virtual void SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor); 					    	 // _3C

    ut::Rect GetLayoutRect() const;

    Pane* GetRootPane() const {
        return mpRootPane;
    }

    GroupContainer* GetGroupContainer() const {
        return mpGroupContainer;
    }

    static MEMAllocator* GetAllocator() {
        return mspAllocator;
    }
    static void SetAllocator(MEMAllocator* pAllocator) {
        mspAllocator = pAllocator;
    }

    static void* AllocMemory(u32 size) {
        return MEMAllocFromAllocator(mspAllocator, size);
    }
    static void FreeMemory(void* pBlock) {
        MEMFreeToAllocator(mspAllocator, pBlock);
    }
	
	template <typename T>
    static void DeleteArray(T *p, size_t n);

    template <typename T>
    static T *NewArray(size_t n);

    template <typename T>
    static void DeleteObj(T *t);

    template <typename T>
    static void DeletePrimArray(T *objAry);

    template <typename T>
    static T *NewObj();

    template <typename T, typename P1>
    static T *NewObj(P1 param1);

    template <typename T, typename P1, typename P2>
    static T *NewObj(P1 param1, P2 param2);

protected:
    static const u32 SIGNATURE_TEXTURELIST = FOURCC('t', 'x', 'l', '1');
    static const u32 SIGNATURE_FONTLIST = FOURCC('f', 'n', 'l', '1');
    static const u32 SIGNATURE_MATERIALLIST = FOURCC('m', 'a', 't', '1');

    static const u32 SIGNATURE_ANIMATIONINFO = FOURCC('p', 'a', 'i', '1');

    static const u32 SIGNATURE_PANESTART = FOURCC('p', 'a', 's', '1');
    static const u32 SIGNATURE_PANEEND = FOURCC('p', 'a', 'e', '1');

    static const u32 SIGNATURE_GROUPSTART = FOURCC('g', 'r', 's', '1');
    static const u32 SIGNATURE_GROUPEND = FOURCC('g', 'r', 'e', '1');

protected:
    static Pane* BuildPaneObj(s32 kind, const void* pBinary,
                              const ResBlockSet& rBlockSet);

protected:
    ut::LinkList<AnimTransform, 4> mAnimTransList;   // _08
    Pane* mpRootPane;                				 // _10
    GroupContainer* mpGroupContainer; 				 // _14
    Size mLayoutSize;                				 // _18
    u8 mOriginType;                  				 // _20

    static MEMAllocator* mspAllocator;
};

/******************************************************************************
 *
 * Utility functions
 *
 ******************************************************************************/
//namespace {

template <typename T>
void Layout::DeleteArray(T *p, size_t n) {
    if (p) {
        for (size_t i = 0; i < n; i++) {
            p[i].~T();
        }

        FreeMemory(p);
    }
}

template <typename T>
T *Layout::NewArray(size_t n) {
    T *array = (T *)AllocMemory(n * sizeof(T));
    if (!array) {
        return nullptr;
    }

    for (size_t i = 0; i < n; i++) {
        new (&array[i]) T();
    }

    return array;
}

template <typename T>
void Layout::DeleteObj(T *t) {
    if (t) {
        t->~T();
        FreeMemory(t);
    }
}

template <typename T>
void Layout::DeletePrimArray(T *objAry) {
    if (objAry) {
        FreeMemory(objAry);
    }
}

template <typename T>
T *Layout::NewObj() {
    T *pMem = (T *)AllocMemory(sizeof(T));
    if (pMem) {
        return new (pMem) T();
    } else {
        return nullptr;
    }
}

template <typename T, typename P1>
T *Layout::NewObj(P1 param1) {
    T *pMem = (T *)AllocMemory(sizeof(T));
    if (pMem) {
        return new (pMem) T(param1);
    } else {
        return nullptr;
    }
}

template <typename T, typename P1, typename P2>
T *Layout::NewObj(P1 param1, P2 param2) {
    T *pMem = (T *)AllocMemory(sizeof(T));
    if (pMem) {
        return new (pMem) T(param1, param2);
    } else {
        return nullptr;
    }
}

//} // namespace
} // namespace lyt
} // namespace nw4r

#endif
