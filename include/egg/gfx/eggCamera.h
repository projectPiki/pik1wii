#ifndef EGG_GFX_CAMERA_H
#define EGG_GFX_CAMERA_H

#include <egg/egg_types.h>
#include <egg/math/eggMatrix.h>
#include <egg/math/eggVector.h>
#include <nw4r/math/math_types.h>
#include <nw4r/g3d/g3d_camera.h>
#include <RevoSDK/gx.h>

namespace EGG {

class ProjectionData;

class BaseCamera {
public:
    BaseCamera() {}
	
    virtual Matrix34f &getViewMatrix() = 0;
    virtual const Matrix34f &getViewMatrix() const = 0;
    virtual void updateMatrix();
    virtual void doUpdateMatrix() = 0;
    virtual void loadMatrix() = 0;
    virtual void loadOldMatrix() = 0;
    virtual EGG::Vector3f getPosition() = 0;
    virtual void draw(EGG::BaseCamera*);
    virtual void doDraw() = 0;
    virtual Matrix34f &getViewMatrixOld() = 0;

    void setG3DCamera(nw4r::g3d::Camera&);
    Vector3f getRightVector();
    Vector3f getLookVector();
    Vector3f getUpVector();
	void unProject(Vector2f&, ProjectionData*);

protected:
    Matrix34f mViewMtx;		// _04
    Matrix34f mOtherMtx;	// _34
};

class LookAtCamera : public BaseCamera {
public:
    LookAtCamera() : mPos(0.0f, 10.0f, 0.0f), mAt(0.0f, 0.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f) {}
    
    LookAtCamera& operator=(const LookAtCamera &other) {
        mViewMtx.copyFrom(other.mViewMtx);
        mOtherMtx.copyFrom(other.mOtherMtx);
        mPos = other.mPos;
        mAt = other.mAt;
        mUp = other.mUp;
        return *this;
    }

    virtual Matrix34f &getViewMatrix(){ return mViewMtx; }
    virtual const Matrix34f &getViewMatrix() const { return mViewMtx; }
    virtual void doUpdateMatrix();
    virtual void loadMatrix();
    virtual void loadOldMatrix();
    virtual EGG::Vector3f getPosition() { return mPos; }
    virtual void doDraw();
    virtual Matrix34f &getViewMatrixOld();

    Vector3f getDirection() const 
	{
        return mAt - mPos;
    }

    Vector3f getOtherDirection() const 
	{
        return mPos - mAt;
    }

public:
    Vector3f mPos;		// _64
    Vector3f mAt; 		// _70
    Vector3f mUp;		// _7C
};

} // namespace EGG

#endif