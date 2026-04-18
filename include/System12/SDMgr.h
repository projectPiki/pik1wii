#ifndef _SYSTEM12_SD_MGR_H
#define _SYSTEM12_SD_MGR_H

#include <egg/core/eggSingleton.h>
#include <egg/core/eggStream.h>

namespace System12 {

class SDMgr {
	
EGG_SINGLETON_DECL(SDMgr);

public:

	SDMgr(){}
	
	void initialize();

	
	// _00     = VTBL
	// _00-_10 = EGG::Disposer
};

} // namespace System12

#endif
