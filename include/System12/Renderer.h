#ifndef _SYSTEM12_RENDERER_H
#define _SYSTEM12_RENDERER_H

#include <System12/3D.h>
#include <System12/Window.h>
#include <egg/core/eggSingleton.h>
#include <egg/gfx/eggCamera.h>
#include <egg/gfx/eggProjection.h>
#include <egg/math/eggMatrix.h>
#include <nw4r/g3d.h>

namespace System12 {

class Renderer {
	EGG_SINGLETON_DECL(Renderer);

public:
	struct Arg {
		EGG::BaseCamera* mCamera;
		Window* mWindow;
		EGG::ProjectionData* mProjection;
		u32 mMaxScnObj;
		u32 mNumLightObj;
		EGG::Heap* mHeap;
	};

	Renderer();

	virtual void calc();
	virtual void draw();
	virtual void setup_draw();

	void init(Arg&);
	void push_back(Model*);
	void raw_draw();

	EGG::ProjectionData* getProjection() { return mProjection; }
	void setProjection(EGG::ProjectionData* proj) { mProjection = proj; }
	// EGG::BaseCamera* getCamera() { return mCamera; }
	Window* getWindow() { return mWindow; }
	void setWindow(Window* window) { mWindow = window; }
	RootScene& getRootScene() { return mScene; }

	RootScene mScene;                 // _14
	EGG::BaseCamera* mCamera;         // _18
	Window* mWindow;                  // _1C
	EGG::ProjectionData* mProjection; // _20
	u32 mMaxScnObj;                   // _24
	u32 mNumLightObj;                 // _28
	EGG::Heap* mHeap;                 // _2C
};

} // namespace System12

#endif
