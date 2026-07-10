#include "RevoSDK/os.h"
#include "System12/Renderer.h"
#include "egg/core/eggAllocator.h"
#include "egg/core/eggThread.h"
#include "egg/gfx/eggCamera.h"

namespace System12 {

EGG_SINGLETON_IMPL(9, Renderer);

Renderer::Renderer()
{
	mCamera     = nullptr;
	mWindow     = nullptr;
	mProjection = nullptr;
	mHeap       = nullptr;
}

void Renderer::init(Arg& arg)
{
	mCamera      = arg.mCamera;
	mWindow      = arg.mWindow;
	mProjection  = arg.mProjection;
	mMaxScnObj   = arg.mMaxScnObj;
	mNumLightObj = arg.mNumLightObj;
	mHeap        = arg.mHeap;
	mScene.construct(arg.mMaxScnObj, arg.mNumLightObj, arg.mHeap);
	mScene.getRoot()->SetCurrentCamera(0);
}

void Renderer::push_back(Model* model)
{
	mScene.getRoot()->Insert(mScene.getRoot()->Size(), model->getModel());
}

void Renderer::calc()
{
	mScene.getRoot()->UpdateFrame();
	mScene.getRoot()->CalcWorld();
	mScene.getRoot()->CalcMaterial();
}

void Renderer::setup_draw()
{
	nw4r::g3d::Camera camera = mScene.getRoot()->GetCurrentCamera();
	mWindow->setG3DCamera(camera);
	mProjection->setG3DCamera(camera);
	mCamera->setG3DCamera(camera);
}

void Renderer::draw()
{
	setup_draw();
	raw_draw();
}

void Renderer::raw_draw()
{
	nw4r::g3d::G3dReset();
	mScene.getRoot()->CalcView();
	mScene.getRoot()->GatherDrawScnObj();
	mScene.getRoot()->ZSort();
	mScene.getRoot()->DrawOpa();
	mScene.getRoot()->DrawXlu();
}
} // namespace System12
