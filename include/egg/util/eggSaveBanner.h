#ifndef EGG_UTIL_SAVEBANNER_H
#define EGG_UTIL_SAVEBANNER_H

#include <egg/egg_types.h>
#include <egg/core/eggHeap.h>
#include <RevoSDK/nand.h>
#include <RevoSDK/tpl.h>

#include <wchar.h>

namespace EGG {
class SaveBanner {
	SaveBanner(Heap *pHeap){
		mFileNum = 8;
		mErrorOccured = false;
		mErrorCode = 0;
		mBanner = static_cast<NANDBanner*>(Heap::alloc(sizeof(NANDBanner), 32, pHeap));
	}
    virtual ~SaveBanner();
	
	SaveBanner* create(Heap *pHeap);
	void initBanner(TPLPalette*, u32, const wchar_t*, const wchar_t*);
	s32 createBannerFile(u8, u8);
	void saveBannerFile();
	void saveBannerFile(int);
	
	static SaveBanner* sSaveBanner;

private:
    u32 mFileNum;
    bool mErrorOccured;
    int mErrorCode;
    NANDBanner *mBanner;
};
}

#endif