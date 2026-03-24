#ifndef NW4R_G3D_ANM_CHR_H
#define NW4R_G3D_ANM_CHR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmchr.h>

namespace nw4r {
namespace g3d {

/******************************************************************************
 *
 * AnmObjChr
 *
 ******************************************************************************/
// Forward declarations
class AnmObjChrRes;

class AnmObjChr : public AnmObj {
public:
    enum BindOption { BIND_ONE, BIND_PARTIAL, NUM_OF_BIND_OPTION };

public:
    AnmObjChr(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; // _C
    virtual ~AnmObjChr() {}                                     // _10

    virtual void SetFrame(f32 frame) = 0; // _1C
    virtual f32 GetFrame() const = 0;     // _20
    virtual void UpdateFrame() = 0;       // _24

    virtual void SetUpdateRate(f32 rate) = 0; // _28
    virtual f32 GetUpdateRate() const = 0;    // _2C

    virtual bool Bind(const ResMdl mdl) = 0; // _30
    virtual void Release();                  // _34

    virtual const ChrAnmResult* GetResult(ChrAnmResult* pResult,
                                          u32 idx) = 0; // _38

    virtual AnmObjChrRes* Attach(int idx, AnmObjChrRes* pRes); // _3C
    virtual AnmObjChrRes* Detach(int idx);                     // _40
    virtual void DetachAll();                                  // _44

    virtual void SetWeight(int idx, f32 weight); // _48
    virtual f32 GetWeight(int idx) const;        // _4C

    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option) = 0; // _50
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option) = 0; // _54

    bool TestExistence(u32 idx) const;
    bool TestDefined(u32 idx) const;

    void UseQuaternionBlend(bool enable) {
        SetAnmFlag(FLAG_USE_QUATERNION_ROTATION_BLEND, enable);
    }

    void UseAccurateScaleBlend(bool enable) {
        SetAnmFlag(FLAG_USE_ACCURATE_SCALE_BLEND, enable);
    }

protected:
    enum BindingFlag {
        BINDING_ID_MASK = (1 << 14) - 1,
        BINDING_INVALID = (1 << 14),
        BINDING_UNDEFINED = (1 << 15),
    };

protected:
    static const int DEFAULT_MAX_CHILDREN = 4;

protected:
    int mNumBinding;      // _10
    u16* const mpBinding; // _14

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChr, AnmObj);
};

/******************************************************************************
 *
 * AnmObjChrNode
 *
 ******************************************************************************/
class AnmObjChrNode : public AnmObjChr {
public:
    AnmObjChrNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                  AnmObjChrRes** ppChildrenBuf, int numChildren);


    virtual void G3dProc(u32 task, u32 param, void* pInfo); 		// _0C
    virtual ~AnmObjChrNode();                               		// _10
    virtual void SetFrame(f32 frame); 								// _1C
    virtual f32 GetFrame() const;     								// _20
    virtual void UpdateFrame();       								// _24
    virtual void SetUpdateRate(f32 rate); 							// _28
    virtual f32 GetUpdateRate() const;    							// _2C
    virtual bool Bind(const ResMdl mdl); 							// _30
    virtual void Release();              							// _34
    virtual AnmObjChrRes* Attach(int idx, AnmObjChrRes* pRes); 		// _3C
    virtual AnmObjChrRes* Detach(int idx);                     		// _40
    virtual void DetachAll();                                  		// _44
    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option); 							// _50
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option); 						// _54

protected:
    int mChildrenArraySize;         // _18
    AnmObjChrRes** mpChildrenArray; // _1C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrNode, AnmObjChr);
};

/******************************************************************************
 *
 * AnmObjChrBlend
 *
 ******************************************************************************/
class AnmObjChrBlend : public AnmObjChrNode {
    static AnmObjChrBlend* Construct(MEMAllocator* pAllocator, u32* pSize,
                                     ResMdl mdl, int numChildren);

    AnmObjChrBlend(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                   AnmObjChrRes** ppChildrenBuf, int numChildren,
                   f32* pWeightBuf);

    virtual ~AnmObjChrBlend() {} 									// _10
    virtual const ChrAnmResult* GetResult(ChrAnmResult* pResult,
                                          u32 idx); 				// _38
    virtual void SetWeight(int idx, f32 weight); 					// _48
    virtual f32 GetWeight(int idx) const;        					// _4C

private:
    f32* mpWeightArray; // _20

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrBlend, AnmObjChrNode);
};

/******************************************************************************
 *
 * AnmObjChrRes
 *
 ******************************************************************************/
class AnmObjChrRes : public AnmObjChr, protected FrameCtrl {
public:
    static AnmObjChrRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                   ResAnmChr chr, ResMdl mdl, bool cache);

    AnmObjChrRes(MEMAllocator* pAllocator, ResAnmChr chr, u16* pBindingBuf,
                 int numBinding, ChrAnmResult* pCacheBuf);
    
	
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjChrRes() {}                             				// _10
    virtual void SetFrame(f32 frame); 									// _1C
    virtual f32 GetFrame() const;     									// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    using AnmObjChr::Release;            								// _40
    virtual const ChrAnmResult* GetResult(ChrAnmResult* pResult,
                                          u32 idx); 					// _38
    virtual bool Bind(const ResMdl mdl, u32 target,
                      BindOption option); 								// _50
    virtual void Release(const ResMdl mdl, u32 target,
                         BindOption option); 							// _54

    void UpdateCache();

    ResAnmChr GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmChr mRes;                    // _2C
    ChrAnmResult* const mpResultCache; // _30

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjChrRes, AnmObjChr);
};

} // namespace g3d
} // namespace nw4r

#endif
