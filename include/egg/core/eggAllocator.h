#ifndef _EGG_CORE_ALLOCATOR_H
#define _EGG_CORE_ALLOCATOR_H

#include <egg/egg_types.h>
#include <RevoSDK/mem.h>

namespace EGG {

class Heap;

struct Allocator : public MEMAllocator {
	Allocator(Heap*, s32);

	virtual ~Allocator();

	virtual void* alloc(u32);
	virtual void free(void*);

	// _00-_10 = MEMAllocator
	// _10     = VTBL
	Heap* mHeap; // _14
	u32 mAlign;  // _18
};

} // namespace EGG

#endif
