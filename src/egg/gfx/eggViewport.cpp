#include <egg/gfx/eggViewport.h>

namespace EGG {


Viewport::Viewport() : mScreenWidth(0.0f), mScreenHeight(0.0f), mAspectRatio(0.0f) {}

void Viewport::set(int left, int up, int right, int down) {
    mBox.mMin.x = left;
    mBox.mMin.y = up;
    mBox.mMax.x = right;
    mBox.mMax.y = down;

    calc();
}

void Viewport::setLUAndWH(f32 left, f32 up, f32 width, f32 height) {
    mBox.mMin.x = left;
    mBox.mMin.y = up;
    mBox.mMax.x = left + width;
    mBox.mMax.y = up + height;

    calc();
}

void Viewport::calc() {
    mScreenWidth = mBox.getSizeX();
    mScreenHeight = mBox.getSizeY();
    mAspectRatio = mScreenWidth / mScreenHeight;
}

void Viewport::setG3DCamera(nw4r::g3d::Camera &camera) {
    f32 xOrigin = mBox.mMin.x;
    f32 yOrigin = mBox.mMin.y;
    f32 width = mScreenWidth;
    f32 height = mScreenHeight;

    camera.SetScissor(xOrigin, yOrigin, width, height);
    camera.SetViewport(xOrigin, yOrigin, width, height);
}

} // namespace EGG