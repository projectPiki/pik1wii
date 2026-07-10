#ifndef EGG_GFX_PROJECTION_H
#define EGG_GFX_PROJECTION_H

#include <egg/egg_types.h>
#include <egg/math/eggMatrix.h>
#include <egg/math/eggVector.h>
//#include <egg/gfx/eggCamera.h>
#include <egg/gfx/eggViewport.h>
#include <egg/prim/eggBitFlag.h>
#include <nw4r/math/math_types.h>
#include <nw4r/g3d/g3d_camera.h>
#include <RevoSDK/gx.h>

namespace EGG {

class BaseCamera;


class ProjectionData {
public:

	ProjectionData()
		: mViewport(nullptr)
	{
		//: mViewport(nullptr)
	}
	
	
    virtual bool isOrtho() const { return false; };
    virtual void setProjection();
    virtual void calcMatrix();
    virtual Vector2f projectToScreen(Vector3f&);
    virtual Vector3f unProjectToXYPlane(BaseCamera*, const Vector2f&, f32);
    virtual Vector3f unProjectToXZPlane(BaseCamera*, const Vector2f&, f32);
    virtual Vector3f unProjectToCameraCoordinates(const Vector2f&);
    virtual void setDefault() {}
    virtual void setG3DCamera(nw4r::g3d::Camera&);
    virtual void loadProjectionCamMatrix(Matrix34f&);
	
	void unProject(BaseCamera*, const Vector2f&);
	float getWideTVMagX();
	
	bool isWideTV() 
	{
		return sWideTV;
	}
	
	float getNear()
	{
		return mNear;
	}
	float getFar()
	{
		return mFar;
	}
	
	
	
	static float sWideTVMagX;
	static bool sWideTV;


    nw4r::math::MTX44 mMtx; // _00 // Possibly EGG::Matrix44f
    Viewport *mViewport; 	// _44
    f32 mNear; 				// _48
    f32 mFar; 				// _4C
};

class PerspectiveData : public ProjectionData {
public:

	PerspectiveData();
    // Overrides
    virtual void setProjection();
    virtual void calcMatrix();
    virtual Vector2f projectToScreen(Vector3f &) {}
    virtual Vector3f unProjectToCameraCoordinates(const Vector2f &);
    virtual void setG3DCamera(nw4r::g3d::Camera &);
    virtual void loadProjectionCamMatrix(Matrix34f &) {}
	
	void setAspect(f32);

private:
    f32 mFovy; 				// _50
    f32 mFovySin; 			// _54 // TODO better name?
    f32 mFovyCos; 			// _58 // TODO better name?
    f32 mFovyTan; 			// _5C // TODO better name?
    f32 mAspect;			// _60
    Vector2f mVanishPoint;  // _64
    TBitFlag<u8> mFlag; 	// _6C
};

class OrthoData : public ProjectionData {
   
    OrthoData();
	
	
    virtual bool isOrtho() const { return true; }
    virtual void setProjection();
    virtual void calcMatrix() {}
    virtual Vector2f projectToScreen(Vector3f &);
    virtual Vector3f unProjectToXYPlane(BaseCamera *, const Vector2f &, f32 z);
    virtual Vector3f unProjectToXZPlane(BaseCamera *, const Vector2f &, f32 y);
    virtual Vector3f unProjectToCameraCoordinates(const Vector2f &);
    virtual void setDefault();
    virtual void setG3DCamera(nw4r::g3d::Camera &);
    virtual void loadProjectionCamMatrix(Matrix34f &);

 
    virtual void setProjectionForLytAdjust();
	
	void setTBLR(f32, f32, f32, f32);

private:
    f32 mTop; 		// _50
    f32 mBottom; 	// _54
    f32 mLeft; 		// _58
    f32 mRight; 	// _5C
};
}

#endif