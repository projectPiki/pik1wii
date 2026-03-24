#ifndef NW4R_G3D_ANM_SHP_H
#define NW4R_G3D_ANM_SHP_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmshp.h>

namespace nw4r {
namespace g3d {

/******************************************************************************
 *
 * AnmObjShp
 *
 ******************************************************************************/
// Forward declarations
class AnmObjShpRes;

class AnmObjShp : public AnmObj {
public:
    AnmObjShp(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    	
	virtual void G3dProc(u32 task, u32 param, void* pInfo) = 0; 	// _0C
    virtual ~AnmObjShp() {}                                     	// _10
    virtual void SetFrame(f32 frame) = 0; 							// _1C
    virtual f32 GetFrame() const = 0;     							// _20
    virtual void UpdateFrame() = 0;       							// _24
    virtual void SetUpdateRate(f32 rate) = 0; 						// _28
    virtual f32 GetUpdateRate() const = 0;   	 					// _2C
    virtual bool Bind(const ResMdl mdl) = 0; 						// _30
    virtual void Release();                  						// _34
    virtual const ShpAnmResult* GetResult(ShpAnmResult* pResult,
                                          u32 idx) = 0; 			// _38
    virtual AnmObjShpRes* Attach(int idx, AnmObjShpRes* pRes); 		// _3C
    virtual AnmObjShpRes* Detach(int idx);                     		// _40
    virtual void DetachAll();                                  		// _44
    virtual void SetWeight(int idx, f32 weight); 					// _48
    virtual f32 GetWeight(int idx) const;        					// _4C

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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShp, AnmObj);
};

/******************************************************************************
 *
 * AnmObjShpNode
 *
 ******************************************************************************/
class AnmObjShpNode : public AnmObjShp {
public:
    AnmObjShpNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                  AnmObjShpRes** ppChildrenBuf, int numChildren);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 			// _0C
    virtual ~AnmObjShpNode();                               			// _10
    virtual void SetFrame(f32 frame); 									// _1C
    virtual f32 GetFrame() const;     									// _20
    virtual void UpdateFrame();       									// _24
    virtual void SetUpdateRate(f32 rate); 								// _28
    virtual f32 GetUpdateRate() const;    								// _2C
    virtual bool Bind(const ResMdl mdl); 								// _30
    virtual void Release();              								// _34
    virtual AnmObjShpRes* Attach(int idx, AnmObjShpRes* pRes); 			// _3C
    virtual AnmObjShpRes* Detach(int idx);                     			// _40
    virtual void DetachAll();                                  			// _44

protected:
    int mChildrenArraySize;         // _18
    AnmObjShpRes** mpChildrenArray; // _1C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShpNode, AnmObjShp);
};

/******************************************************************************
 *
 * AnmObjShpBlend
 *
 ******************************************************************************/
class AnmObjShpBlend : public AnmObjShpNode {
    static AnmObjShpBlend* Construct(MEMAllocator* pAllocator, u32* pSize,
                                     ResMdl mdl, int numChildren);

    AnmObjShpBlend(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding,
                   AnmObjShpRes** ppChildrenBuf, int numChildren,
                   f32* pWeightBuf);

    virtual ~AnmObjShpBlend() {} 											// _10
    virtual const ShpAnmResult* GetResult(ShpAnmResult* pResult,
                                          u32 idx); 						// _38
    virtual void SetWeight(int idx, f32 weight); 							// _48
    virtual f32 GetWeight(int idx) const;        							// _4C

private:
    f32* mpWeightArray; // _20

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShpBlend, AnmObjShpNode);
};

/******************************************************************************
 *
 * AnmObjShpRes
 *
 ******************************************************************************/
class AnmObjShpRes : public AnmObjShp, protected FrameCtrl {
public:
    static AnmObjShpRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                   ResAnmShp shp, ResMdl mdl, bool cache);

    AnmObjShpRes(MEMAllocator* pAllocator, ResAnmShp shp, u16* pBindingBuf,
                 ShpAnmVtxSet* pVtxSetBuf, int numBinding,
                 ShpAnmResult* pCacheBuf);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 		// _0C
    virtual ~AnmObjShpRes() {}                              		// _10
    virtual void SetFrame(f32 frame); 								// _1C
    virtual f32 GetFrame() const;     								// _20
    virtual void UpdateFrame();       								// _24
    virtual void SetUpdateRate(f32 rate); 							// _28
    virtual f32 GetUpdateRate() const;    							// _2C
    virtual bool Bind(const ResMdl mdl); 							// _30
    virtual const ShpAnmResult* GetResult(ShpAnmResult* pResult,
                                          u32 idx); 				// _38

    void UpdateCache();

    ResAnmShp GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmShp mRes;                    // _2C
    ShpAnmVtxSet* const mpVtxSetArray; // _30
    ShpAnmResult* const mpResultCache; // _34

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjShpRes, AnmObjShp);
};

} // namespace g3d
} // namespace nw4r

#endif
