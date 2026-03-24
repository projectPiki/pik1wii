#ifndef NW4R_G3D_ANM_VIS_H
#define NW4R_G3D_ANM_VIS_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/res/g3d_resanmvis.h>

namespace nw4r {
namespace g3d {

// Forward declarations
class AnmObjVisRes;
class AnmObjVis;
class ResMdl;

void ApplyVisAnmResult(ResMdl mdl, AnmObjVis* pObj);
void ApplyVisAnmResult(u8* pByteVec, ResMdl mdl, AnmObjVis* pObj);

/******************************************************************************
 *
 * AnmObjVis
 *
 ******************************************************************************/
class AnmObjVis : public AnmObj {
public:
    AnmObjVis(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 		// _0C
    virtual ~AnmObjVis() {}                                 		// _10
    virtual void SetFrame(f32 frame) = 0; 							// _1C
    virtual f32 GetFrame() const = 0;     							// _20
    virtual void UpdateFrame() = 0;       							// _24
    virtual void SetUpdateRate(f32 rate) = 0; 						// _28
    virtual f32 GetUpdateRate() const = 0;    						// _2C
    virtual bool Bind(const ResMdl mdl) = 0; 						// _30
    virtual void Release();                  						// _34
    virtual bool GetResult(u32 idx) = 0; 							// _38
    virtual AnmObjVisRes* Attach(int idx, AnmObjVisRes* pRes); 		// _3C
    virtual AnmObjVisRes* Detach(int idx);                    		// _40
    
	
	void DetachAll();

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

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVis, AnmObj);
};

/******************************************************************************
 *
 * AnmObjVisNode
 *
 ******************************************************************************/
class AnmObjVisNode : public AnmObjVis {
public:
    AnmObjVisNode(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding);
    
	virtual void G3dProc(u32 task, u32 param, void* pInfo); 	// _0C
    virtual ~AnmObjVisNode();                               	// _10
    virtual void SetFrame(f32 frame); 							// _1C
    virtual f32 GetFrame() const;     							// _20
    virtual void UpdateFrame();       							// _24
    virtual void SetUpdateRate(f32 rate); 						// _28
    virtual f32 GetUpdateRate() const;    						// _2C
    virtual bool Bind(const ResMdl mdl); 						// _30
    virtual void Release();              						// _34
    virtual bool GetResult(u32 idx) = 0; 						// _38
    virtual AnmObjVisRes* Attach(int idx, AnmObjVisRes* pRes); 	// _3C
    virtual AnmObjVisRes* Detach(int idx);                     	// _40

protected:
    AnmObjVisRes* mpChildren[MAX_CHILD]; // _18

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisNode, AnmObjVis);
};

/******************************************************************************
 *
 * AnmObjVisOR
 *
 ******************************************************************************/
class AnmObjVisOR : public AnmObjVisNode {
public:
    static AnmObjVisOR* Construct(MEMAllocator* pAllocator, u32* pSize,
                                  ResMdl mdl);

    AnmObjVisOR(MEMAllocator* pAllocator, u16* pBindingBuf, int numBinding)
        : AnmObjVisNode(pAllocator, pBindingBuf, numBinding) {}

    virtual ~AnmObjVisOR() {} 			// _10
    virtual bool GetResult(u32 idx); 	// _38

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisOR, AnmObjVisNode);
};

/******************************************************************************
 *
 * AnmObjVisRes
 *
 ******************************************************************************/
class AnmObjVisRes : public AnmObjVis, protected FrameCtrl {
public:
    static AnmObjVisRes* Construct(MEMAllocator* pAllocator, u32* pSize,
                                   ResAnmVis vis, ResMdl mdl);

    AnmObjVisRes(MEMAllocator* pAllocator, ResAnmVis vis, u16* pBindingBuf,
                 int numBinding);
    
	virtual ~AnmObjVisRes() {} 					// _10
    virtual void SetFrame(f32 frame); 			// _1C
    virtual f32 GetFrame() const;     			// _20
    virtual void UpdateFrame();       			// _24
    virtual void SetUpdateRate(f32 rate); 		// _28
    virtual f32 GetUpdateRate() const;   	 	// _2C
    virtual bool Bind(const ResMdl mdl); 		// _30
    virtual bool GetResult(u32 idx); 			// _38

    ResAnmVis GetResAnm() {
        return mRes;
    }

    void SetPlayPolicy(PlayPolicyFunc pFunc) {
        FrameCtrl::SetPlayPolicy(pFunc);
    }

private:
    ResAnmVis mRes; // _2C

    NW4R_G3D_RTTI_DECL_DERIVED(AnmObjVisRes, AnmObjVis);
};

} // namespace g3d
} // namespace nw4r

#endif
