#include "types.h"
#include <egg/core/eggAllocator.h>
#include <egg/core/eggExpHeap.h>
#include <stl/new.h>

namespace EGG {

ExpHeap::~ExpHeap()
{
	dispose();
	MEMDestroyExpHeap(mHeapHandle);
}

ExpHeap* ExpHeap::create(void* pHeapStart, u32 size, u16 opt)
{
	ExpHeap* heap = nullptr;

	void* pHeapBuffer = pHeapStart;
	void* pHeapEnd    = ROUND_DOWN_PTR(addOffset(pHeapStart, size), 4);

	pHeapStart = ROUND_UP_PTR(pHeapStart, 4);

	if (pHeapStart > pHeapEnd || nw4r::ut::GetOffsetFromPtr(pHeapStart, pHeapEnd) < sizeof(MEMiHeapHead)) {
		return nullptr;
	}

	MEMiHeapHead* heapHandle
	    = MEMCreateExpHeapEx(addOffset(pHeapStart, 0x38), nw4r::ut::GetOffsetFromPtr(pHeapStart, pHeapEnd) - 0x38, opt);
	if (heapHandle) {
		Heap* r31 = Heap::findContainHeap(pHeapStart);

		heap              = new (pHeapStart) ExpHeap(heapHandle);
		heap->mHeapBuffer = pHeapBuffer;
		heap->mParentHeap = r31;

		if (Heap::sCreateCallback) {
			Heap::sCreateCallback(heap);
		}
	}

	return heap;
}

#pragma dont_inline on

ExpHeap* ExpHeap::create(u32 size, Heap* parent, u16 opt)
{
	ExpHeap* heap = nullptr;

	if (!parent) {
		parent = Heap::sCurrentHeap;
	}
	if (size == 0xFFFFFFFF) {
		size = parent->getAllocatableSize();
	}
	void* pBlock = parent->alloc(size);
	if (pBlock) {

		heap = ExpHeap::create(pBlock, size, opt);

		if (!heap) {
			parent->free(pBlock);
		}
	}

	return heap;
}

#pragma dont_inline reset

void ExpHeap::destroy()
{
	if (Heap::sDestroyCallback) {
		Heap::sDestroyCallback(this);
	}

	Heap* parent = findParentHeap();

	this->~ExpHeap();

	if (parent) {
		parent->free(this);
	}
}

void* ExpHeap::alloc(u32 size, s32 align)
{
	if (mFlags.on(1)) {
		OSErrorLine(186, "DAME DAME\n");
	}
	return MEMAllocFromExpHeapEx(mHeapHandle, size, align);
}

void ExpHeap::free(void* pBlock)
{
	MEMFreeToExpHeap(mHeapHandle, pBlock);
}

u32 ExpHeap::resizeForMBlock(void* pBlock, u32 size)
{
	return MEMResizeForMBlockExpHeap(mHeapHandle, pBlock, size);
}

u32 ExpHeap::getTotalFreeSize()
{
	return MEMGetTotalFreeSizeForExpHeap(mHeapHandle);
}

u32 ExpHeap::getAllocatableSize(s32 align)
{
	return MEMGetAllocatableSizeForExpHeapEx(mHeapHandle, align);
}

// TODO: find what 0x38 is
u32 ExpHeap::adjust()
{
	u32 size = MEMAdjustExpHeap(mHeapHandle) + 0x38;

	if (size > 0x38 && mParentHeap) {
		mParentHeap->resizeForMBlock(mHeapBuffer, size);
		return size;
	}
	return 0;
}

void ExpHeap::initAllocator(Allocator* alloc, s32 align)
{
	MEMInitAllocatorForExpHeap(alloc, mHeapHandle, align);
}

} // namespace EGG