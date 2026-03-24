#ifndef NW4R_G3D_SCN_ROOT_H
#define NW4R_G3D_SCN_ROOT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_fog.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_state.h>

namespace nw4r {
namespace g3d {

// Forward declarations
class AnmScn;

/******************************************************************************
 *
 * ScnRoot
 *
 ******************************************************************************/
class ScnRoot : public ScnGroup {
public:
    enum ScnRootFlag { SCNROOTFLAG_FORCE_RESMDLDRAWMODE = (1 << 0) };

public:
    static ScnRoot* Construct(MEMAllocator* pAllocator, u32* pSize,
                              u32 maxChildren, u32 maxScnObj, u32 numLightObj,
                              u32 numLightSet);

    static ScnRoot* Construct(MEMAllocator* pAllocator, u32* pSize,
                              u32 maxChildren, u32 maxScnObj) {
        return Construct(pAllocator, pSize, maxChildren, maxScnObj,
                         G3DState::NUM_LIGHT, G3DState::NUM_LIGHT_SET);
    }

    ScnRoot(MEMAllocator* pAllocator, IScnObjGather* pGather,
            ScnObj** ppChildrenBuf, u32 maxChildren, u32 numLight,
            u32 numLightSet, LightObj* pLightObjBuf, AmbLightObj* pAmbObjBuf,
            LightSetData* pLightSetBuf);

    virtual void G3dProc(u32 task, u32 param, void* pInfo); // _0C
    virtual ~ScnRoot();                                     // _10

    Camera GetCamera(int idx);
    Camera GetCurrentCamera();
    void SetCurrentCamera(int idx);

    Fog GetFog(int idx);
    LightSet GetLightSet(int idx);

    void UpdateFrame();
    void SetGlbSettings();

    void CalcAnmScn();
    void CalcWorld();
    void CalcMaterial();
    void CalcVtx();
    void CalcView();

    void GatherDrawScnObj();
    void ZSort();
    void DrawOpa();
    void DrawXlu();

    u32 TestScnRootFlag(ScnRootFlag flag) const {
        return (mScnRootFlags & flag) != 0;
    }
    void SetScnRootFlag(ScnRootFlag flag, u32 on) {
        if (on) {
            mScnRootFlags |= flag;
        } else {
            mScnRootFlags &= ~flag;
        }
    }

    int GetCurrentCameraID() const {
        return mCurrentCameraID;
    }

    LightSetting& GetLightSetting() {
        return mLightSetting;
    }

private:
    IScnObjGather* mpCollection;              // _E8
    ResMdlDrawMode mDrawMode;                 // _EC
    u32 mScnRootFlags;                        // _F0
    u8 mCurrentCameraID;                      // _F4
    u8 PADDING_0xF5;                          // _F5
    u8 PADDING_0xF6;                          // _F6
    u8 PADDING_0xF7;                          // _F7
    CameraData mCamera[G3DState::NUM_CAMERA]; // _F8
    FogData mFog[G3DState::NUM_FOG];          // _2278
    LightSetting mLightSetting;               // _2878
    AnmScn* mpAnmScn;                         // _2888

    NW4R_G3D_RTTI_DECL_DERIVED(ScnRoot, ScnGroup);
};

/******************************************************************************
 *
 * ScnObjGather
 *
 ******************************************************************************/
class ScnObjGather : public IScnObjGather {
public:
    ScnObjGather(ScnObj** ppBufOpa, ScnObj** ppBufXlu, u32 capacity);

    virtual ~ScnObjGather() {} 										// _08
    virtual CullingStatus Add(ScnObj* pObj, bool opa, bool xlu); 	// _0C
    virtual void Clear() { mNumScnObjOpa = mNumScnObjXlu = 0; } 	// _10
    virtual void ZSort(); 											// _14
    virtual void Sort(); 											// _18
    virtual void Sort(LessThanFunc pOpaFunc,
                      LessThanFunc pXluFunc); 						// _1C
    virtual void DrawOpa(ResMdlDrawMode* pForceMode); 				// _20
    virtual void DrawXlu(ResMdlDrawMode* pForceMode); 				// _24
	virtual CheckStatus CheckScnObj(ScnObj *); 						// _28

protected:
    ScnObj** mpArrayOpa; // _04
    ScnObj** mpArrayXlu; // _08
    u32 mSizeScnObj;     // _0C
    u32 mNumScnObjOpa;   // _10
    u32 mNumScnObjXlu;   // _14
};

} // namespace g3d
} // namespace nw4r

#endif
