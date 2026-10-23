#include "System12/Heap.h"
#include "sysNew.h"

namespace System12 {

void Heap::initAllocator(EGG::Allocator* pAllocator, s32 align)
{
}

void* Heap::alloc(u32 size, s32 align)
{
	return new (align) u8[size];
}

u32 Heap::resizeForMBlock(void* pBlock, u32 size)
{
}

u32 Heap::getAllocatableSize(s32 align)
{
}

void Heap::free(void* pBlock)
{
}

void Heap::destroy()
{
}

u32 Heap::adjust()
{
	return 0;
}

Heap::~Heap()
{
}

Heap::EHeapKind Heap::getHeapKind() const
{
	return HEAP_KIND_NONE;
}

u32 Heap::getTotalFreeSize()
{
	return 0;
}



} // namespace System12
