#include <egg/core/eggAllocator.h>
#include <egg/core/eggHeap.h>

namespace EGG {

Allocator::Allocator(Heap* heap, s32 align)
    : mHeap(heap)
    , mAlign(align)
{
	mHeap->initAllocator(this, mAlign);
}

Allocator::~Allocator()
{
}

void* Allocator::alloc(u32 size)
{
	return EGG::Heap::alloc(size, mAlign, mHeap);
}

void Allocator::free(void* ptr)
{
	EGG::Heap::free(ptr, mHeap);
}

} // namespace EGG
