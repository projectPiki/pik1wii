#ifndef EGG_GFX_VIEWPORT_H
#define EGG_GFX_VIEWPORT_H

#include <egg/egg_types.h>
#include <egg/math/eggBoundBox.h>
#include <nw4r/g3d/g3d_camera.h>
#include <RevoSDK/gx.h>

namespace EGG {

class Viewport {
public:
    Viewport();

    void set(int left, int up, int right, int down);
    void setLUAndWH(f32 left, f32 up, f32 width, f32 height);
    void calc();

    void setupGXViewport();
    void setDefault();
    void setupGXScissor();
    void setupGX();

    void setG3DCamera(nw4r::g3d::Camera &camera);
	
	float getWidth()
	{
		return mScreenWidth;
	}
	float getHeight()
	{
		return mScreenHeight;
	}

private:
    BoundBox2f mBox; 		// _00
    f32 mScreenWidth; 		// _10
    f32 mScreenHeight; 		// _14
    f32 mAspectRatio; 		// _18
};

} // namespace EGG

#endif