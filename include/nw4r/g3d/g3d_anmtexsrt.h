#ifndef NW4R_G3D_ANM_TEX_SRT_H
#define NW4R_G3D_ANM_TEX_SRT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmtexsrt.h>
#include <nw4r/g3d/res/g3d_resmat.h>

namespace nw4r {
namespace g3d {

void ApplyTexSrtAnmResult(ResTexSrt srt, const TexSrtAnmResult* pResult);
void ApplyTexSrtAnmResult(ResTexSrt srt, ResMatIndMtxAndScale ind,
                          const TexSrtAnmResult* pResult);

/******************************************************************************
 *
 * AnmObjTexSrt
 *
 ******************************************************************************/
// Forward declarations
class AnmObjTexSrtRes;

class AnmObjTexSrt : public AnmObj {
public:
    AnmObjTexSrt(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; 				// _0C
    virtual ~AnmObjTexSrt() {}                                  				// _10
    virtual void SetFrame(f32 frame) = 0; 										// _1C
    virtual f32 GetFrame() const = 0;     										// _20
    virtual void UpdateFrame() = 0;       										// _24
    virtual void SetUpdateRate(f32 rate) = 0; 									// _28
    virtual f32 GetUpdateRate() const = 0;   	 								// _2C
    virtual bool Bind(const ResMdl mdl) = 0; 									// _30
    virtual void Release();                  									// _34
    virtual const TexSrtAnmResult* GetResult(TexSrtAnmResult* pResult,
                                             u32 idx) = 0; 						// _38
    virtual AnmObjTexSrtRes* Attach(int idx, AnmObjTexSrtRes* pRes); 			// _3C
    virtual AnmObjTexSrtRes* Detach(int idx);                        			// _40
    virtual void DetachAll();                                        			// _44

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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrt, AnmObj);
};

/******************************************************************************
 *
 * AnmObjTexSrtNode
 *
 ******************************************************************************/
class AnmObjTexSrtNode : public AnmObjTexSrt {
public:
    AnmObjTexSrtNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                     AnmObjTexSrtRes** ppChildrenBuf, int numChildren);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 					// _0C
    virtual ~AnmObjTexSrtNode();                            					// _10
    virtual void SetFrame(f32 frame); 											// _1C
    virtual f32 GetFrame() const;     											// _20
    virtual void UpdateFrame();       											// _24
    virtual void SetUpdateRate(f32 rate); 										// _28
    virtual f32 GetUpdateRate() const;    										// _2C
    virtual bool Bind(const ResMdl mdl); 										// _30
    virtual void Release();              										// _34
    virtual AnmObjTexSrtRes* Attach(int idx, AnmObjTexSrtRes* pRes); 			// _3C
    virtual AnmObjTexSrtRes* Detach(int idx);                        			// _40
    virtual void DetachAll();                                        			// _44

protected:
    int mChildrenArraySize;            // _18
    AnmObjTexSrtRes** mpChildrenArray; // _1C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtNode, AnmObjTexSrt);
};

/******************************************************************************
 *
 * AnmObjTexSrtOverride
 *
 ******************************************************************************/
class AnmObjTexSrtOverride : public AnmObjTexSrtNode {
    static AnmObjTexSrtOverride* Construct(MEMAllocator* pAllocator, u32* pSize,
                                           ResMdl mdl, int numChildren);

    AnmObjTexSrtOverride(MEMAllocator* pAllocator, u16* pBindingBuf,
                         int numBinding, AnmObjTexSrtRes** ppChildrenBuf,
                         int numChildren)
        : AnmObjTexSrtNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf,
                           numChildren) {}

    virtual ~AnmObjTexSrtOverride() {} 											// _10
    virtual const TexSrtAnmResult* GetResult(TexSrtAnmResult* pResult,
                                             u32 idx); 							// _38

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtOverride, AnmObjTexSrtNode);
};

/******************************************************************************
 *
 * AnmObjTexSrtRes
 *
 ******************************************************************************/
class AnmObjTexSrtRes : public AnmObjTexSrt, protected FrameCtrl {
public:
    static AnmObjTexSrtRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                      ResAnmTexSrt srt, ResMdl mdl, bool cache);

    AnmObjTexSrtRes(MEMAllocator* pAllocator, ResAnmTexSrt srt,
                    u16* pBindingBuf, int numBinding,
                    TexSrtAnmResult* pCacheBuf);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjTexSrtRes() {}                           			// _10
    virtual void SetFrame(f32 frame); 									// _1C
    virtual f32 GetFrame() const;     									// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    virtual const TexSrtAnmResult* GetResult(TexSrtAnmResult* pResult,
                                             u32 idx); 					// _38

    void UpdateCache();

    ResAnmTexSrt GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmTexSrt mRes;                    // _2C
    TexSrtAnmResult* const mpResultCache; // _30

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjTexSrtRes, AnmObjTexSrt);
};

} // namespace g3d
} // namespace nw4r

#endif
