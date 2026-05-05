#ifndef _SYSTEM12_DVD_LOADER_H
#define _SYSTEM12_DVD_LOADER_H

#include <egg/core/eggHeap.h>

namespace System12 {

class DvdLoader {
public:
	struct Arg {
		Arg()
		{
			mHeap     = nullptr;
			mFilePath = nullptr;
		}
		const char* mFilePath;
		EGG::Heap* mHeap;
	};

	static u8* loadSZS(Arg&);
};

} // namespace System12

#endif
