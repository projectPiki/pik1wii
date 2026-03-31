#ifndef _EGG_CORE_EXPHEAP_H
#define _EGG_CORE_EXPHEAP_H

#include <egg/core/eggHeap.h>

namespace EGG {

struct ExpHeap : public Heap {
	inline explicit ExpHeap(MEMiHeapHead* pHeapHandle)
	    : Heap(pHeapHandle)
	{
	}

	static ExpHeap* create(void* pHeapStart, u32 size, u16 opt = 0);
	static ExpHeap* create(u32 size, Heap* parent, u16 opt = 0);

	virtual ~ExpHeap();                                                // _08
	virtual EHeapKind getHeapKind() const { return HEAP_KIND_EXPAND; } // _0C
	virtual void initAllocator(Allocator* pAllocator, s32 align = 4);  // _10
	virtual void* alloc(u32 size, s32 align = 4);                      // _14
	virtual void free(void* pBlock);                                   // _18
	virtual void destroy();                                            // _1C
	virtual u32 resizeForMBlock(void* pBlock, u32 size);               // _20
	virtual u32 getTotalFreeSize();                                    // _24
	virtual u32 getAllocatableSize(s32 align = 4);                     // _28
	virtual u32 adjust();                                              // _2C
};

} // namespace EGG

#endif
