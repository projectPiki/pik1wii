#ifndef EGG_CORE_MUTEX_H
#define EGG_CORE_MUTEX_H

#include <egg/egg_types.h>

namespace EGG {
class Mutex  {
public:

	Mutex() { init(); }
	
	virtual ~Mutex(){}
	
	void unlock() { OSUnlockMutex(&mMutex); }
	void lock() { OSLockMutex(&mMutex); }
	void init() { OSInitMutex(&mMutex); }
	
	OSMutex mMutex;
};

} // namespace EGG

#endif
