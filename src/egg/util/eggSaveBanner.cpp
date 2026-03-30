#include <egg/util/eggSaveBanner.h>

namespace EGG {

SaveBanner* SaveBanner::sSaveBanner;
	
SaveBanner* SaveBanner::create(Heap* heap) 
{
	if (!sSaveBanner){
		if (!heap) {
			heap = Heap::getCurrentHeap();
		}
	
		sSaveBanner  = new (heap, 0x4) SaveBanner(heap);
	}
	
	return sSaveBanner;
}


void SaveBanner::initBanner(TPLPalette* palette, u32 flag, const wchar_t* title, const wchar_t* comment) 
{

	mFileNum -= 1;
	NANDInitBanner(mBanner, flag, title, comment);
	TPLBind(palette);
	TPLDescriptor* descriptor = TPLGet(palette, 0);
	memcpy(mBanner->bannerTexture, descriptor->textureHeader->data, sizeof(mBanner->bannerTexture));
	u8 icon = 0;
	u8 speed = 0;
	for (int i = 0; i < mFileNum; i++){
		descriptor = TPLGet(palette, i);
		if (descriptor){
			memcpy(mBanner->iconTexture[i], descriptor->textureHeader->data, sizeof(mBanner->iconTexture[i]));
			mBanner->iconSpeed = mBanner->iconSpeed & ~(3 << speed) | (2 << speed);
		}
		icon += i;
		speed += 2;
	}
	
	if (mFileNum < 8) {
		mBanner->iconSpeed = mBanner->iconSpeed & ~(3 << (mFileNum << 1));
	}
	return;
}

SaveBanner::~SaveBanner() 
{
	if (mBanner) {
		Heap::free(mBanner, nullptr);
	}

}


} // namespace EGG