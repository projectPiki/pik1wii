#include "types.h"
#include <RevoSDK/mem.h>
#include <egg/core/eggAllocator.h>
#include <egg/core/eggExpHeap.h>
#include <egg/core/eggThread.h>


namespace EGG {

nw4r::ut::List Heap::sHeapList;
OSMutex Heap::sRootMutex;
Heap* Heap::sCurrentHeap;
int Heap::sIsHeapListInitialized;
Heap* Heap::sAllocatableHeap;
ErrorCallback Heap::sErrorCallback;
HeapAllocCallback Heap::sAllocCallback;

void* Heap::sErrorCallbackArg;
void* Heap::sAllocCallbackArg;
void (*Heap::sCreateCallback)(void*);
void (*Heap::sDestroyCallback)(void*);

Thread* Heap::sAllocatableThread;

void Heap::initialize()
{
	NW4R_UT_LIST_INIT(sHeapList, Heap);
	OSInitMutex(&sRootMutex);
	sIsHeapListInitialized = true;
}

Heap::Heap(MEMiHeapHead* pHeapHandle)
    : mHeapHandle(pHeapHandle)
    , mHeapBuffer(nullptr)
    , mParentHeap(nullptr)
{
	mName  = "NoName";
	mFlags = 0;

	NW4R_UT_LIST_INIT(mDisposerList, Disposer);

	OSLockMutex(&sRootMutex);
	nw4r::ut::List_Append(&sHeapList, this);
	OSUnlockMutex(&sRootMutex);
}

Heap::~Heap()
{
	OSLockMutex(&sRootMutex);
	nw4r::ut::List_Remove(&sHeapList, this);
	OSUnlockMutex(&sRootMutex);
}

void* Heap::alloc(size_t size, int align, Heap* heap)
{
	Heap* currentHeap = sCurrentHeap;
	Thread* thread    = Thread::findThread(OSGetCurrentThread());
	if (sAllocatableThread) {
		OSGetCurrentThread();
	}

	if (thread) {
		Heap* newHeap = thread->getAllocatableHeap();
		if (newHeap) {
			currentHeap = newHeap;
			heap        = newHeap;
		}
	}

	if (sAllocatableHeap) {
		if (currentHeap && !heap) {
			heap = currentHeap;
		}

		if (heap != sAllocatableHeap) {
			OSReport("cannot allocate from heap %x(%s) : allocatable heap is %x(%s)\n", heap, heap->getName(), sAllocatableHeap,
			         sAllocatableHeap->getName());
			Heap* threadHeap = thread ? thread->getAllocatableHeap() : nullptr;
			OSReport("\tthread heap=%x\n", threadHeap);
			const char* threadHeapName = thread ? thread->getAllocatableHeap() ? thread->getAllocatableHeap()->getName() : "none" : "none";
			OSReport("\tthread heap=%s\n", threadHeapName);
			if (sErrorCallback) {
				HeapErrorArg errorArg;
				errorArg.msg      = "disable_but";
				errorArg.userdata = sErrorCallbackArg;
				sErrorCallback(&errorArg);
			}

			dumpAll();
			return nullptr;
		}
	}

	if (sAllocCallback) {
		HeapAllocArg arg;
		arg.heap    = heap ? heap : currentHeap;
		arg.size    = size;
		arg.align   = align;
		arg.userArg = sAllocCallbackArg;
		sAllocCallback(&arg);
	}

	if (heap) {
		return heap->alloc(size, align);
	}

	if (currentHeap) {
		void* block = currentHeap->alloc(size, align);
		if (!block) {
			OSReport("heap (%p):(%.1fMBytes free %d)->alloc(size(%d:%.1fMBytes),%d "
			         "align)\n",
			         currentHeap, currentHeap->getTotalSize() / (1024.0f * 1024.0f), currentHeap->getAllocatableSize(4), size,
			         size / (1024.0f * 1024.0f), align);
			dumpAll();
		}
		return block;
	}

	OSReport("cannot allocate %d from heap %x\n", size, heap);
	dumpAll();
	return nullptr;
}

Heap* Heap::findHeap(MEMiHeapHead* pHeapHandle)
{
	Heap* pContainHeap = nullptr;
	OSLockMutex(&sRootMutex);

	if (sIsHeapListInitialized) {
		NW4R_UT_LIST_FOREACH(Heap, it, sHeapList, {
			if (it->mHeapHandle == pHeapHandle) {
				pContainHeap = it;
				break;
			}
		})
	}

	OSUnlockMutex(&sRootMutex);
	return pContainHeap;
}

Heap* Heap::findParentHeap()
{
	return mParentHeap;
}

Heap* Heap::findContainHeap(const void* pBlock)
{
	Heap* pContainHeap        = nullptr;
	MEMiHeapHead* pHeapHandle = MEMFindContainHeap(pBlock);

	if (pHeapHandle != nullptr) {
		pContainHeap = findHeap(pHeapHandle);
	}

	return pContainHeap;
}

void Heap::free(void* pBlock, Heap* pHeap)
{
	if (pHeap == nullptr) {
		MEMiHeapHead* pHeapHandle = MEMFindContainHeap(pBlock);
		if (pHeapHandle == nullptr) {
			return;
		}

		pHeap = findHeap(pHeapHandle);
		if (pHeap == nullptr) {
			return;
		}
	}

	pHeap->free(pBlock);
}

void Heap::dispose()
{
	Disposer* pDisposer = nullptr;

	while ((pDisposer = static_cast<Disposer*>(nw4r::ut::List_GetFirst(&mDisposerList))) != nullptr) {

		pDisposer->~Disposer();
	}
}

void Heap::dumpAll()
{
	Heap* heap = nullptr;
	u32 mem[2] = { 0, 0 };

	while ((heap = (Heap*)nw4r::ut::List_GetNext(&sHeapList, heap)) != nullptr) {
		Heap* childHeap = nullptr;
		if ((u32)heap < 0x90000000) {
			mem[0] += heap->getAllocatableSize(4);
		} else {
			mem[1] += heap->getAllocatableSize(4);
		}

		while ((childHeap = (Heap*)nw4r::ut::List_GetNext(&sHeapList, childHeap)) != nullptr) {
			if (heap->findParentHeap() == childHeap) {
				break;
			}
		}
	}
}

Heap* Heap::becomeCurrentHeap()
{

	OSLockMutex(&sRootMutex);

	Heap* pOldHeap = sCurrentHeap;
	sCurrentHeap   = this;

	OSUnlockMutex(&sRootMutex);
	return pOldHeap;
}

} // namespace EGG

void* operator new(size_t size)
{
	return EGG::Heap::alloc(size, 4, nullptr);
}

void* operator new(size_t size, void* p)
{
	return p;
}

void* operator new(size_t size, EGG::Heap* pHeap, int align)
{
	return EGG::Heap::alloc(size, align, pHeap);
}

void* operator new[](size_t size)
{
	return EGG::Heap::alloc(size, 4, nullptr);
}

void* operator new[](size_t size, int align)
{
	return EGG::Heap::alloc(size, align, nullptr);
}

void* operator new[](size_t size, EGG::Heap* pHeap, int align)
{
	return EGG::Heap::alloc(size, align, pHeap);
}

void operator delete(void* pBlock)
{
	EGG::Heap::free(pBlock, nullptr);
}

void operator delete[](void* pBlock)
{
	EGG::Heap::free(pBlock, nullptr);
}
