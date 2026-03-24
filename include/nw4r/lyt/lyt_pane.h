#ifndef NW4R_LYT_PANE_H
#define NW4R_LYT_PANE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_animation.h>
#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_types.h>
#include <nw4r/math.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

// Forward declarations
class DrawInfo;
class Material;
class AnimResource;

/******************************************************************************
 *
 * AnimOption
 *
 ******************************************************************************/
enum AnimOption {
    ANIMOPTION_SKIP_INVISIBLE = (1 << 0),
};

namespace detail {

/******************************************************************************
 *
 * PaneBase
 *
 ******************************************************************************/
class PaneBase {
public:
    PaneBase();
    virtual ~PaneBase(); 	// _08

public:
    NW4R_UT_LINKLIST_NODE_DECL(); 	// _04
};

} // namespace detail

namespace res {

struct ExtUserDataList {
    DataBlockHeader blockHeader; 	 // _00
    u16 num;                     	 // _08
    u16 padding;                 	 // _0A
};

/******************************************************************************
 *
 * PAN1 binary layout
 *
 ******************************************************************************/
struct Pane {
    static const u32 SIGNATURE = FOURCC('p', 'a', 'n', '1');

    DataBlockHeader blockHeader;                  // _00
    u8 flag;                                      // _08
    u8 basePosition;                              // _09
    u8 alpha;                                     // _0A
    u8 padding;                                   // _0B
    char name[NW4R_RES_NAME_SIZE];                // _0C
    char userData[NW4R_LYT_PANE_USERDATA_LEN];    // _1C
    math::VEC3 translate;                         // _24
    math::VEC3 rotate;                            // _30
    math::VEC2 scale;                             // _3C
    Size size;                                    // _44
};

} // namespace res

/******************************************************************************
 *
 * Pane
 *
 ******************************************************************************/
class Pane : public detail::PaneBase {
public:
    NW4R_UT_RTTI_DECL(Pane);

private:
    enum {
        BIT_VISIBLE,
        BIT_INFLUENCED_ALPHA,
        BIT_LOCATION_ADJUST,
    };

    // Need the typedef before the class definition is complete
#ifdef __MWERKS__
        typedef ut::LinkList< Pane, offsetof(detail::PaneBase, node) > PaneList;
		//typedef ut::LinkList<Pane, offsetof(PaneBase, node)> PaneList;
#else
        typedef ut::LinkList< Pane, 0 > PaneList;
#endif

public:
    explicit Pane(const res::Pane* pRes);
    virtual ~Pane(); 																				// _08
    virtual void CalculateMtx(const DrawInfo& rInfo); 												// _10
    virtual void Draw(const DrawInfo& rInfo);     												    // _14
    virtual void DrawSelf(const DrawInfo& rInfo); 												    // _18
    virtual void Animate(u32 option);     															// _1C
    virtual void AnimateSelf(u32 option); 															// _20
    virtual ut::Color GetVtxColor(u32 idx) const;  													// _24
    virtual void SetVtxColor(u32 idx, ut::Color color); 										    // _28
    virtual u8 GetColorElement(u32 idx) const;          								    		// _2C
    virtual void SetColorElement(u32 idx, u8 value);    							    		    // _30
    virtual u8 GetVtxColorElement(u32 idx) const;       							    			// _34
    virtual void SetVtxColorElement(u32 idx, u8 value); 							    			// _38
    virtual Pane* FindPaneByName(const char* pName, bool recursive); 					    		// _3C
    virtual Material* FindMaterialByName(const char* pName, bool recursive); 						// _40
    virtual void BindAnimation(AnimTransform* pAnimTrans,  bool recursive, bool disable); 			// _44
    virtual void UnbindAnimation(AnimTransform* pAnimTrans, bool recursive); 						// _48
    virtual void UnbindAllAnimation(bool recursive);            									// _4C
    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans); 									// _50
    virtual AnimationLink *FindAnimationLinkSelf(AnimTransform *pAnimTrans);                     	// _54
    virtual AnimationLink *FindAnimationLinkSelf(const AnimResource &animRes);                  	// _58
    virtual void SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable, bool bRecursive);  	// _5C
    virtual void SetAnimationEnable(const AnimResource &animRes, bool bEnable, bool bRecursive); 	// _60
    virtual u8 GetMaterialNum() const;                                                          	// _64
    virtual Material *GetMaterial() const;                                                       	// _68
    virtual Material *GetMaterial(u32 idx) const;                                                	// _6C
    virtual void LoadMtx(const DrawInfo &drawInfo);                                             	// _70

    
	
	void AppendChild(Pane* pChild);
    void RemoveChild(Pane* pChild);
	
	void CalculateMtxChild(const DrawInfo& rInfo);

    ut::Rect GetPaneRect(const DrawInfo& rInfo) const;

    void AddAnimationLink(AnimationLink* pAnimLink);

    math::VEC2 GetVtxPos() const;

    Pane* GetParent() const {
        return mpParent;
    }

    PaneList& GetChildList() {
        return mChildList;
    }

    void SetSRTElement(u32 idx, f32 value) {
        reinterpret_cast<f32*>(&mTranslate)[idx] = value;
    }

    const math::VEC3& GetTranslate() const {
        return mTranslate;
    }
    void SetTranslate(const math::VEC2& rTransXY) {
        SetTranslate(math::VEC3(rTransXY.x, rTransXY.y, 0.0f));
    }
    void SetTranslate(const math::VEC3& rTrans) {
        mTranslate = rTrans;
    }

    const math::VEC3& GetRotate() const {
        return mRotate;
    }
    void SetRotate(const math::VEC3& rRotate) {
        mRotate = rRotate;
    }

    const math::VEC2& GetScale() const {
        return mScale;
    }
    void SetScale(const math::VEC2& rScale) {
        mScale = rScale;
    }

    const Size& GetSize() const {
        return mSize;
    }
    void SetSize(const Size& rSize) {
        mSize = rSize;
    }

    const math::MTX34& GetMtx() const {
        return mMtx;
    }
    void SetMtx(const math::MTX34& rMtx) {
        mMtx = rMtx;
    }

    const math::MTX34& GetGlobalMtx() const {
        return mGlbMtx;
    }
    void SetGlobalMtx(const math::MTX34& rGlbMtx) {
        mGlbMtx = rGlbMtx;
    }

    u8 GetAlpha() const {
        return mAlpha;
    }
    void SetAlpha(u8 alpha) {
        mAlpha = alpha;
    }

    u8 GetGlbAlpha() const {
        return mGlbAlpha;
    }
    void SetGlbAlpha(u8 alpha) {
        mGlbAlpha = alpha;
    }

    u8 GetBasePositionH() const {
        return detail::GetHorizontalPosition(mBasePosition);
    }
    void SetBasePositionH(u8 position) {
        detail::SetHorizontalPosition(&mBasePosition, position);
    }

    u8 GetBasePositionV() const {
        return detail::GetVerticalPosition(mBasePosition);
    }
    void SetBasePositionV(u8 position) {
        detail::SetVerticalPosition(&mBasePosition, position);
    }

    bool IsVisible() const {
        return detail::TestBit(mFlag, BIT_VISIBLE);
    }
    void SetVisible(bool visible) {
        detail::SetBit(&mFlag, BIT_VISIBLE, visible);
    }

    bool IsInfluencedAlpha() const {
        return detail::TestBit(mFlag, BIT_INFLUENCED_ALPHA);
    }
    void SetInfluencedAlpha(bool influenced) {
        detail::SetBit(&mFlag, BIT_INFLUENCED_ALPHA, influenced);
    }

    bool IsLocationAdjust() const {
        return detail::TestBit(mFlag, BIT_LOCATION_ADJUST);
    }
    void SetLocationAdjust(bool adjust) {
        detail::SetBit(&mFlag, BIT_LOCATION_ADJUST, adjust);
    }
	
	const res::ExtUserDataList *GetExtUserDataList() const {
        return mpExtUserDataList;
    }
    void SetExtUserDataList(const res::ExtUserDataList *pBlock) {
        mpExtUserDataList = pBlock;
    }

    const char* GetName() const {
        return mName;
    }
    void SetName(const char* pName);

    const char* GetUserData() const {
        return mUserData;
    }
    void SetUserData(const char* pUserData);

    bool IsUserAllocated() const {
        return mbUserAllocated;
    }

protected:
    Pane* mpParent;        								// _0C
    PaneList mChildList; 								// _10
    AnimationLinkList mAnimList;						// _1C
    Material* mpMaterial;   					    	// _28
    math::VEC3 mTranslate; 					    	    // _2C
    math::VEC3 mRotate; 					    	    // _38
    math::VEC2 mScale; 					    	        // _44
    Size mSize;   					    	            // _4C
    math::MTX34 mMtx;  					    	        // _54
    math::MTX34 mGlbMtx;  					    	    // _84
	const res::ExtUserDataList *mpExtUserDataList;  	// _B4
    u8 mAlpha;       		    	   				    // _B8
    u8 mGlbAlpha;    		    	   				    // _B9
    u8 mBasePosition;   		    	   			    // _BA
    u8 mFlag;     		    	   				        // _BB
    char mName[NW4R_RES_NAME_SIZE + 1]; 	            // _BC
    char mUserData[NW4R_LYT_PANE_USERDATA_LEN + 1];     // _CD
    bool mbUserAllocated; 	                            // _D6
    u8 PADDING_0xD7;  	                                // _D7

protected:
    void InsertChild(PaneList::Iterator next, Pane* pChild);

private:
    void Init();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Pane);

} // namespace lyt
} // namespace nw4r

#endif
