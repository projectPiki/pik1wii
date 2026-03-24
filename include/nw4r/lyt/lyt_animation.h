#ifndef NW4R_LYT_ANIMATION_H
#define NW4R_LYT_ANIMATION_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_group.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_types.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

namespace res {

/******************************************************************************
 *
 * AnimationBlock
 *
 ******************************************************************************/
struct AnimationBlock {
    DataBlockHeader blockHeader; // _00
    u16 frameSize;               // _08
    u8 loop;                     // _0A
    u8 PADDING_0xB;              // _0B
    u16 fileNum;                 // _0C
    u16 animContNum;             // _0E
    u32 animContOffsetsOffset;   // _10
};

struct AnimationTagBlock {
    DataBlockHeader blockHeader; // _00
    u16 tagOrder;                // _08
    u16 groupNum;                // _0A
    u32 nameOffset;              // _0C
    u32 groupsOffset;            // _10
    u16 startFrame;              // _14
    u16 endFrame;                // _16
    u8 flag;                     // _18
    u8 padding[3];               // _19
};

struct AnimationShareBlock {
    DataBlockHeader blockHeader; // _00
    u32 animShareInfoOffset;     // _04
    u16 shareNum;                // _0C
    u8 padding[2];               // _0E
};

/******************************************************************************
 *
 * AnimationInfo
 *
 ******************************************************************************/
struct AnimationInfo {
    u32 kind;                  // _00
    u8 num;                    // _04
    u8 PADDING_0x5[0x8 - 0x5]; // _05

    static const u32 SIGNATURE_ANMPANESRT = FOURCC('R', 'L', 'P', 'A');
    static const u32 SIGNATURE_ANMPANEVIS = FOURCC('R', 'L', 'V', 'I');
    static const u32 SIGNATURE_ANMVTXCLR = FOURCC('R', 'L', 'V', 'C');

    static const u32 SIGNATURE_ANMMATCLR = FOURCC('R', 'L', 'M', 'C');
    static const u32 SIGNATURE_ANMTEXSRT = FOURCC('R', 'L', 'T', 'S');
    static const u32 SIGNATURE_ANMTEXPAT = FOURCC('R', 'L', 'T', 'P');
    static const u32 SIGNATURE_ANMINDTEXSRT = FOURCC('R', 'L', 'I', 'M');
};

/******************************************************************************
 *
 * AnimationContent
 *
 ******************************************************************************/
struct AnimationContent {
    static const int NAME_LEN =
        MAX(NW4R_RES_NAME_SIZE, NW4R_LYT_MATERIAL_NAME_LEN);

    enum AnimType { ANIMTYPE_PANE, ANIMTYPE_MATERIAL, ANIMTYPE_MAX };

    char name[NAME_LEN];          // _00
    u8 num;                       // _14
    u8 type;                      // _15
    u8 PADDING_0x16[0x18 - 0x16]; // _16
};

} // namespace res

/******************************************************************************
 *
 * AnimTransform
 *
 ******************************************************************************/
class AnimTransform {
public:
    AnimTransform();
	
    virtual ~AnimTransform(); 												// _08
    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor) = 0; 				// _0C
	virtual void SetResource(
        const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor,
        u16 animNum
    ) = 0;  																// _10
    virtual void Bind(Pane* pPane, bool recursive, bool disable) = 0; 		// _14
    virtual void Bind(Material* pMaterial, bool disable) = 0;         		// _18
    virtual void Animate(u32 idx, Pane* pPane) = 0;         				// _1C
    virtual void Animate(u32 idx, Material* pMaterial) = 0; 				// _20

    u16 GetFrameSize() const;

    bool IsLoopData() const;

    f32 GetFrame() const {
        return mFrame;
    }
    void SetFrame(f32 frame) {
        mFrame = frame;
    }

    const res::AnimationBlock* GetAnimResource() const {
        return mpRes;
    }

    f32 GetFrameMax() const {
        return static_cast<f32>(GetFrameSize());
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL(); // _04

protected:
    const res::AnimationBlock* mpRes; // _0C
    f32 mFrame;                       // _10
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(AnimTransform);

/******************************************************************************
 *
 * AnimTransformBasic
 *
 ******************************************************************************/
class AnimTransformBasic : public AnimTransform {
public:
    AnimTransformBasic();
	
    virtual ~AnimTransformBasic(); 																			// _08
    virtual void SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor); 				// _0C
	virtual void SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor, u16 animNum); // _10
    virtual void Bind(Pane* pPane, bool recursive, bool bDisable); 											// _10
    virtual void Bind(Material* pMaterial, bool disable);         											// _14
    virtual void Animate(u32 idx, Pane* pPane);         													// _18
    virtual void Animate(u32 idx, Material* pMaterial); 													// _1C
	
	template <typename T>
    AnimationLink *Bind(T *pTarget, AnimationLink *pAnimLink, u16 idx, bool bDisable);
	
	AnimationLink *FindUnbindLink(AnimationLink *pLink) const;

    AnimationLink *GetAnimLinkAry() const {
        return mAnimLinkAry;
    }

protected:
    void** mpFileResAry;         // _14
    AnimationLink* mAnimLinkAry; // _18
    u16 mAnimLinkNum;            // _1C
};

class AnimResource {
public:
    AnimResource();

    explicit AnimResource(const void* anmResBuf) { Set(anmResBuf); }

    void Set(const void*);
	
	void Init();
	
    const res::AnimationBlock* GetResourceBlock() const { return mpResBlock; }

    bool IsDescendingBind() const;

    u16 GetGroupNum() const;
    const AnimationGroupRef* GetGroupArray() const;

    u16 GetAnimationShareInfoNum() const;
    const AnimationShareInfo* GetAnimationShareInfoArray() const;

    u16 CalcAnimationNum(Pane*, bool) const;
    u16 CalcAnimationNum(Material*) const;
    u16 CalcAnimationNum(Group*, bool) const;

    const res::BinaryFileHeader* mpFileHeader;
    const res::AnimationBlock* mpResBlock;
    const res::AnimationTagBlock* mpTagBlock;
    const res::AnimationShareBlock* mpShareBlock;
};

namespace detail {
    class AnimPaneTree {
    public:
        AnimPaneTree() { Init(); }

        AnimPaneTree(Pane* pTargetPane, const AnimResource& animRes) {
            Init();
            Set(pTargetPane, animRes);
        }

        bool IsEnabled() const { return mLinkNum > 0; }
		
		static u16 FindAnimContent(const res::AnimationBlock *pAnimBlock, const char *animContName, u8 animContType);

        AnimTransform* Bind(Layout*, Pane*, ResourceAccessor*) const;

        void Set(Pane*, const AnimResource&);
        void Init();

        AnimResource mAnimRes;
        u16 mAnimPaneIdx;
        u16 mLinkNum;
        u16 mAnimMatIdxs[9];
        u8 mAnimMatCnt;
    };

    AnimationLink *FindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, AnimTransform *pAnimTrans);
	AnimationLink *FindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, const AnimResource &animRes);
	void UnbindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, AnimTransform *pAnimTrans);
	void UnbindAnimationLink(ut::LinkList<AnimationLink, 0> *pAnimList, const AnimResource &animRes);
}  // namespace detail

/******************************************************************************
 *
 * AnimTargetPane
 *
 ******************************************************************************/
enum AnimTargetPane {
    ANIMTARGET_PANE_TRANSX,
    ANIMTARGET_PANE_TRANSY,
    ANIMTARGET_PANE_TRANSZ,

    ANIMTARGET_PANE_ROTX,
    ANIMTARGET_PANE_ROTY,
    ANIMTARGET_PANE_ROTZ,

    ANIMTARGET_PANE_SCALEX,
    ANIMTARGET_PANE_SCALEY,

    ANIMTARGET_PANE_SIZEX,
    ANIMTARGET_PANE_SIZEY,

    ANIMTARGET_PANE_MAX,

    ANIMTARGET_PANE_COLOR_ALPHA = 16,
    ANIMTARGET_PANE_COLOR_MAX,
};

/******************************************************************************
 *
 * AnimTargetVtxColor
 *
 ******************************************************************************/
enum AnimTargetVtxColor {
    ANIMTARGET_VERTEXCOLOR_LT_RED,
    ANIMTARGET_VERTEXCOLOR_LT_GREEN,
    ANIMTARGET_VERTEXCOLOR_LT_BLUE,
    ANIMTARGET_VERTEXCOLOR_LT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RT_RED,
    ANIMTARGET_VERTEXCOLOR_RT_GREEN,
    ANIMTARGET_VERTEXCOLOR_RT_BLUE,
    ANIMTARGET_VERTEXCOLOR_RT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_LB_RED,
    ANIMTARGET_VERTEXCOLOR_LB_GREEN,
    ANIMTARGET_VERTEXCOLOR_LB_BLUE,
    ANIMTARGET_VERTEXCOLOR_LB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RB_RED,
    ANIMTARGET_VERTEXCOLOR_RB_GREEN,
    ANIMTARGET_VERTEXCOLOR_RB_BLUE,
    ANIMTARGET_VERTEXCOLOR_RB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_MAX
};

/******************************************************************************
 *
 * AnimTargetMatColor
 *
 ******************************************************************************/
enum AnimTargetMatColor {
    ANIMTARGET_MATCOLOR_MATR,
    ANIMTARGET_MATCOLOR_MATG,
    ANIMTARGET_MATCOLOR_MATB,
    ANIMTARGET_MATCOLOR_MATA,

    ANIMTARGET_MATCOLOR_TEV0R,
    ANIMTARGET_MATCOLOR_TEV0G,
    ANIMTARGET_MATCOLOR_TEV0B,
    ANIMTARGET_MATCOLOR_TEV0A,

    ANIMTARGET_MATCOLOR_TEV1R,
    ANIMTARGET_MATCOLOR_TEV1G,
    ANIMTARGET_MATCOLOR_TEV1B,
    ANIMTARGET_MATCOLOR_TEV1A,

    ANIMTARGET_MATCOLOR_TEV2R,
    ANIMTARGET_MATCOLOR_TEV2G,
    ANIMTARGET_MATCOLOR_TEV2B,
    ANIMTARGET_MATCOLOR_TEV2A,

    ANIMTARGET_MATCOLOR_TEVK0R,
    ANIMTARGET_MATCOLOR_TEVK0G,
    ANIMTARGET_MATCOLOR_TEVK0B,
    ANIMTARGET_MATCOLOR_TEVK0A,

    ANIMTARGET_MATCOLOR_TEVK1R,
    ANIMTARGET_MATCOLOR_TEVK1G,
    ANIMTARGET_MATCOLOR_TEVK1B,
    ANIMTARGET_MATCOLOR_TEVK1A,

    ANIMTARGET_MATCOLOR_TEVK2R,
    ANIMTARGET_MATCOLOR_TEVK2G,
    ANIMTARGET_MATCOLOR_TEVK2B,
    ANIMTARGET_MATCOLOR_TEVK2A,

    ANIMTARGET_MATCOLOR_TEVK3R,
    ANIMTARGET_MATCOLOR_TEVK3G,
    ANIMTARGET_MATCOLOR_TEVK3B,
    ANIMTARGET_MATCOLOR_TEVK3A,

    ANIMTARGET_MATCOLOR_MAX
};

/******************************************************************************
 *
 * AnimTargetTexSRT
 *
 ******************************************************************************/
enum AnimTargetTexSRT {
    ANIMTARGET_TEXSRT_TRANSX,
    ANIMTARGET_TEXSRT_TRANSY,

    ANIMTARGET_TEXSRT_ROT,

    ANIMTARGET_TEXSRT_SCALEX,
    ANIMTARGET_TEXSRT_SCALEY,

    ANIMTARGET_TEXSRT_MAX
};

/******************************************************************************
 *
 * AnimTargetTexPat
 *
 ******************************************************************************/
enum AnimTargetTexPat {
    ANIMTARGET_TEXPATTURN_IMAGE,

    ANIMTARGET_TEXPATTURN_MAX
};

} // namespace lyt
} // namespace nw4r

#endif
