#ifndef NW4R_G3D_ANM_TEX_PAT_H
#define NW4R_G3D_ANM_TEX_PAT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmtexpat.h>

namespace nw4r {
namespace g3d {

void ApplyTexPatAnmResult(ResTexObj texObj, ResTlutObj tlutObj,
                          const TexPatAnmResult* pResult);

/******************************************************************************
 *
 * AnmObjTexPat
 *
 ******************************************************************************/
// Forward declarations
class AnmObjTexPatRes;

class AnmObjTexPat : public AnmObj {
public:
    AnmObjTexPat(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; 		// _0C
    virtual ~AnmObjTexPat() {}                                  		// _10
    virtual void SetFrame(f32 frame) = 0; 								// _1C
    virtual f32 GetFrame() const = 0;     								// _20
    virtual void UpdateFrame() = 0;       								// _24
    virtual void SetUpdateRate(f32 rate) = 0; 							// _28
    virtual f32 GetUpdateRate() const = 0;    							// _2C
    virtual bool Bind(const ResMdl mdl) = 0; 							// _30
    virtual void Release();                  							// _34
    virtual const TexPatAnmResult* GetResult(TexPatAnmResult* pResult,
                                             u32 idx) = 0; 				// _38
    virtual AnmObjTexPatRes* Attach(int idx, AnmObjTexPatRes* pRes); 	// _3C
    virtual AnmObjTexPatRes* Detach(int idx);                        	// _40
    virtual void DetachAll();                                        	// _44

    bool TestExistence(u32 idx) const;
    bool TestDefined(u32 idx) const;

protected:
    enum BindingFlag {
        BINDING_ID_MASK = (1 << 14) - 1,
        BINDING_INVALID = (1 << 14),
        BINDING_UNDEFINED = (1 << 15),
    };

protected:
    static const int MAX_CHILD = 4;

protected:
    int mNumBinding;      // _10
    u16* const mpBinding; // _14

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPat, AnmObj);
};

/******************************************************************************
 *
 * AnmObjTexPatNode
 *
 ******************************************************************************/
class AnmObjTexPatNode : public AnmObjTexPat {
public:
    AnmObjTexPatNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                     AnmObjTexPatRes** ppChildrenBuf, int numChildren);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjTexPatNode();                            			// _10
    virtual void SetFrame(f32 frame); 									// _1C
    virtual f32 GetFrame() const;     									// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    virtual void Release();              								// _34
    virtual AnmObjTexPatRes* Attach(int idx, AnmObjTexPatRes* pRes); 	// _3C
    virtual AnmObjTexPatRes* Detach(int idx);                        	// _40
    virtual void DetachAll();                                        	// _44

protected:
    int mChildrenArraySize;            // _18
    AnmObjTexPatRes** mpChildrenArray; // _1C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatNode, AnmObjTexPat);
};

/******************************************************************************
 *
 * AnmObjTexPatOverride
 *
 ******************************************************************************/
class AnmObjTexPatOverride : public AnmObjTexPatNode {
    static AnmObjTexPatOverride* Construct(MEMAllocator* pAllocator, u32* pSize,
                                           ResMdl mdl, int numChildren);

    AnmObjTexPatOverride(MEMAllocator* pAllocator, u16* pBindingBuf,
                         int numBinding, AnmObjTexPatRes** ppChildrenBuf,
                         int numChildren)
        : AnmObjTexPatNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf,
                           numChildren) {}

    virtual ~AnmObjTexPatOverride() {} 									// _10
    virtual const TexPatAnmResult* GetResult(TexPatAnmResult* pResult,
                                             u32 idx); 					// _38

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatOverride, AnmObjTexPatNode);
};

/******************************************************************************
 *
 * AnmObjTexPatRes
 *
 ******************************************************************************/
class AnmObjTexPatRes : public AnmObjTexPat, protected FrameCtrl {
public:
    static AnmObjTexPatRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                      ResAnmTexPat pat, ResMdl mdl, bool cache);

    AnmObjTexPatRes(MEMAllocator* pAllocator, ResAnmTexPat pat,
                    u16* pBindingBuf, int numBinding,
                    TexPatAnmResult* pCacheBuf);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjTexPatRes() {}                           			// _10
    virtual void SetFrame(f32 frame);		 							// _1C
    virtual f32 GetFrame() const;     									// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    virtual const TexPatAnmResult* GetResult(TexPatAnmResult* pResult,
                                             u32 idx); 					// _38

    void UpdateCache();

    ResAnmTexPat GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmTexPat mRes;                    // _2C
    TexPatAnmResult* const mpResultCache; // _30

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexPatRes, AnmObjTexPat);
};

} // namespace g3d
} // namespace nw4r

#endif
