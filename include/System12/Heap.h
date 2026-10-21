#ifndef _SYSTEM12_HEAP_H
#define _SYSTEM12_HEAP_H

#include <egg/core/eggHeap.h>

namespace System12 {

struct Heap : public EGG::Heap {

	Heap()
	    : EGG::Heap()
	{
	}

	virtual ~Heap();                                                       // _08
	virtual EHeapKind getHeapKind() const;                                 // _0C
	virtual void initAllocator(EGG::Allocator* pAllocator, s32 align = 4); // _10
	virtual void* alloc(u32 size, s32 align = 4);                          // _14
	virtual void free(void* pBlock);                                       // _18
	virtual void destroy();                                                // _1C
	virtual u32 resizeForMBlock(void* pBlock, u32 size);                   // _20
	virtual u32 getTotalFreeSize();                                        // _24
	virtual u32 getAllocatableSize(s32 align = 4);                         // _28
	virtual u32 adjust();                                                  // _2C
};

} // namespace System12

#endif
