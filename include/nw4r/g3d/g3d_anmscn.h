#ifndef NW4R_G3D_ANM_SCN_H
#define NW4R_G3D_ANM_SCN_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_fog.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_obj.h>

namespace nw4r {
namespace g3d {

// Forward declarations
struct AmbLightAnmResult;
class AnmScnRes;
struct CameraAnmResult;
struct FogAnmResult;
struct LightAnmResult;
class LightSetting;

class AnmScn : public G3dObj {
public:
    explicit AnmScn(MEMAllocator* pAllocator);
    
	virtual ~AnmScn(); 															// _10
    virtual void SetFrame(f32 frame) = 0; 										// _1C
    virtual f32 GetFrame() const = 0;     										// _20
    virtual void SetUpdateRate(f32 rate) = 0; 									// _24
    virtual f32 GetUpdateRate() const = 0;    									// _28
    virtual void UpdateFrame() = 0;           									// _2C
    virtual AnmScnRes* Attach(int idx, AnmScnRes* pRes); 						// _30
    virtual AnmScnRes* Detach();                         						// _34
    virtual u32 GetNumLightSet() const = 0;      								// _38
    virtual u32 GetNumAmbLight() const = 0;      								// _3C
    virtual u32 GetNumDiffuseLight() const = 0;  								// _40
    virtual u32 GetNumSpecularLight() const = 0; 								// _44
    virtual u32 GetNumFog() const = 0;           								// _48
    virtual u32 GetNumCamera() const = 0;        								// _4C
    virtual u32 GetLightSetMaxRefNumber() const = 0;     						// _50
    virtual u32 GetAmbLightMaxRefNumber() const = 0;     						// _54
    virtual u32 GetDiffuseLightMaxRefNumber() const = 0; 						// _58
    virtual u32 GetFogMaxRefNumber() const = 0;          						// _5C
    virtual u32 GetCameraMaxRefNumber() const = 0;       						// _60
    virtual bool GetLightSet(LightSet set, u32 refNumber) = 0; 					// _64
    virtual ut::Color GetAmbLightColor(u32 refNumber) = 0;     					// _68
    virtual void GetLight(LightObj* pDiff, LightObj* pSpec,
                          u32 refNumber) = 0;                 					// _6C
    virtual void GetFog(Fog fog, u32 refNumber) = 0;          					// _70
    virtual bool GetCamera(Camera camera, u32 refNumber) = 0; 					// _74
    virtual u32 GetSpecularLightID(u32 refNumber) const = 0; 					// _78
    virtual bool HasSpecularLight(u32 refNumber) const = 0;  					// _7C
    virtual AmbLightAnmResult* GetAmbLightResult(AmbLightAnmResult* pResult,
                                                 u32 refNumber) = 0; 			// _80
    virtual LightAnmResult* GetLightResult(LightAnmResult* pResult,
                                           u32 refNumber) = 0; 					// _84
    virtual FogAnmResult* GetFogResult(FogAnmResult* pResult,
                                       u32 refNumber) = 0; 						// _88
    virtual CameraAnmResult* GetCameraResult(CameraAnmResult* pResult,
                                             u32 refNumber) = 0; 				// _8C

    void GetLightSetting(LightSetting* pSetting);

private:
    NW4R_G3D_RTTI_DECL_DERIVED(AnmScn, G3dObj);
};

} // namespace g3d
} // namespace nw4r

#endif
