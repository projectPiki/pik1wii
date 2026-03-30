#include <egg/gfx/eggCamera.h>

namespace EGG {

void BaseCamera::updateMatrix() {
    doUpdateMatrix();
}

void BaseCamera::draw(EGG::BaseCamera *cam) {
    cam->loadMatrix();
    doDraw();
}

void BaseCamera::setG3DCamera(nw4r::g3d::Camera &cam) {
    cam.SetCameraMtxDirectly(*reinterpret_cast<const nw4r::math::MTX34*>(&getViewMatrix()));
}

Vector3f BaseCamera::getLookVector()
{
	Matrix34f* mtx = &getViewMatrix();
	return Vector3f(mtx->m[2][0], mtx->m[2][1], mtx->m[2][2]);
}

Vector3f BaseCamera::getRightVector()
{
	Matrix34f* mtx = &getViewMatrix();
	return Vector3f(mtx->m[0][0], mtx->m[0][1], mtx->m[0][2]);
}

Vector3f BaseCamera::getUpVector()
{
	Matrix34f* mtx = &getViewMatrix();
	return Vector3f(mtx->m[1][0], mtx->m[1][1], mtx->m[1][2]);
}

Matrix34f &LookAtCamera::getViewMatrixOld() {
    return mOtherMtx;
}

void LookAtCamera::doUpdateMatrix() {
    mOtherMtx = mViewMtx;

    Vector3f right(mPos);

    right -= mAt;
    right.normalise();

    Vector3f forward(mUp.cross(right).normalize());

    Vector3f up(right.cross(forward));
    up.normalise();

    f32 tx = -forward.dot(mPos);
    f32 ty = -up.dot(mPos);
    f32 tz = -right.dot(mPos);

    f32 t[] = { tx, ty, tz};

    mViewMtx(0, 0) = forward(0);
    mViewMtx(0, 1) = forward(1);
    mViewMtx(0, 2) = forward(2);
    mViewMtx(0, 3) = tx;

    mViewMtx(1, 0) = up(0);
    mViewMtx(1, 1) = up(1);
    mViewMtx(1, 2) = up(2);
    mViewMtx(1, 3) = ty;

    mViewMtx(2, 0) = right(0);
    mViewMtx(2, 1) = right(1);
    mViewMtx(2, 2) = right(2);
    mViewMtx(2, 3) = tz;
}

void LookAtCamera::doDraw() {}

void LookAtCamera::loadMatrix() {
    Matrix34f mtx;
    GXLoadPosMtxImm(mViewMtx.m, 0);
    mViewMtx.inverseTransposeTo(mtx);
    GXLoadNrmMtxImm(mtx.m, 0);
}

void LookAtCamera::loadOldMatrix() {}


} // namespace EGG