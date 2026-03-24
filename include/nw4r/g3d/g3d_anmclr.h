#ifndef NW4R_G3D_ANM_CLR_H
#define NW4R_G3D_ANM_CLR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmclr.h>
#include <nw4r/g3d/res/g3d_resmat.h>

namespace nw4r {
namespace g3d {

void ApplyClrAnmResult(ResMatChan chan, ResMatTevColor tev,
                       const ClrAnmResult* pResult);

/******************************************************************************
 *
 * AnmObjMatClr
 *
 ******************************************************************************/
// Forward declarations
class AnmObjMatClrRes;

class AnmObjMatClr : public AnmObj {
public:
    AnmObjMatClr(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
	
	
    virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; 		// _0C
    virtual ~AnmObjMatClr() {}                                  		// _10
    virtual void SetFrame(f32 frame) = 0; 								// _1C
    virtual f32 GetFrame() const = 0;     								// _20
    virtual void UpdateFrame() = 0;       								// _24
    virtual void SetUpdateRate(f32 rate) = 0; 							// _28
    virtual f32 GetUpdateRate() const = 0;    							// _2C
    virtual bool Bind(const ResMdl mdl) = 0; 							// _30
    virtual void Release();                  							// _34
    virtual const ClrAnmResult* GetResult(ClrAnmResult* pResult,
                                          u32 idx) = 0; 			 	// _38
    virtual AnmObjMatClrRes* Attach(int idx, AnmObjMatClrRes* pRes); 	// _3C
    virtual AnmObjMatClrRes* Detach(int idx);                        	// _40
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
    static const int DEFAULT_MAX_CHILDREN = 4;

protected:
    int mNumBinding;      // _10
    u16* const mpBinding; // _14

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClr, AnmObj);
};

/******************************************************************************
 *
 * AnmObjMatClrNode
 *
 ******************************************************************************/
class AnmObjMatClrNode : public AnmObjMatClr {
public:
    AnmObjMatClrNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                     AnmObjMatClrRes** ppChildrenBuf, int numChildren);
   

    virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjMatClrNode();                            			// _10
    virtual void SetFrame(f32 frame); 									// _1C
    virtual f32 GetFrame() const;    	 								// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    virtual void Release();              								// _34
    virtual AnmObjMatClrRes* Attach(int idx, AnmObjMatClrRes* pRes); 	// _3C
    virtual AnmObjMatClrRes* Detach(int idx);                        	// _40
    virtual void DetachAll();                                        	// _44

protected:
    int mChildrenArraySize;            // _18
    AnmObjMatClrRes** mpChildrenArray; // _1C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrNode, AnmObjMatClr);
};

/******************************************************************************
 *
 * AnmObjMatClrOverride
 *
 ******************************************************************************/
class AnmObjMatClrOverride : public AnmObjMatClrNode {
    static AnmObjMatClrOverride* Construct(MEMAllocator* pAllocator, u32* pSize,
                                           ResMdl mdl, int numChildren);

    AnmObjMatClrOverride(MEMAllocator* pAllocator, u16* pBindingBuf,
                         int numBinding, AnmObjMatClrRes** ppChildrenBuf,
                         int numChildren)
        : AnmObjMatClrNode(pAllocator, pBindingBuf, numBinding, ppChildrenBuf,
                           numChildren) {}

    virtual ~AnmObjMatClrOverride() {} 									// _10
    virtual const ClrAnmResult* GetResult(ClrAnmResult* pResult,
                                          u32 idx); 					// _38

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrOverride, AnmObjMatClrNode);
};

/******************************************************************************
 *
 * AnmObjMatClrRes
 *
 ******************************************************************************/
class AnmObjMatClrRes : public AnmObjMatClr, protected FrameCtrl {
public:
    static AnmObjMatClrRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                      ResAnmClr clr, ResMdl mdl, bool cache);

    AnmObjMatClrRes(MEMAllocator* pAllocator, ResAnmClr clr, u16* pBindingBuf,
                    int numBinding, ClrAnmResult* pCacheBuf);

    virtual void G3dProc(u32 task, u32 param, void* pInfo); 		// _0C
    virtual ~AnmObjMatClrRes() {}                           		// _10
    virtual void SetFrame(f32 frame); 								// _1C
    virtual f32 GetFrame() const;     								// _20
    virtual void UpdateFrame();       								// _24
    virtual void SetUpdateRate(f32 rate);	 						// _28
    virtual f32 GetUpdateRate() const;    							// _2C
    virtual bool Bind(const ResMdl mdl); 							// _30
    virtual const ClrAnmResult* GetResult(ClrAnmResult* pResult,
                                          u32 idx); 				// _38

    void UpdateCache();

    ResAnmClr GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmClr mRes;                    // _2C
    ClrAnmResult* const mpResultCache; // _30

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjMatClrRes, AnmObjMatClr);
};

} // namespace g3d
} // namespace nw4r

#endif
