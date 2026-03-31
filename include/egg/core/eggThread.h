#ifndef EGG_CORE_THREAD_H
#define EGG_CORE_THREAD_H
#include <egg/egg_types.h>

#include <nw4r/ut.h>

#include <RevoSDK/os.h>

namespace EGG {

// Forward declarations
class Heap;

class Thread {
public:
    Thread(u32 stackSize, int msgCount, int priority, Heap *heap);
    Thread(OSThread *osThread, int msgCount);
    static Thread *findThread(OSThread *thread);
	
	virtual ~Thread(); // _08

    virtual void* run() { return nullptr; } // _0C
    virtual void onEnter() { } 				// _10
    virtual void onExit() { }  				// _14
	
	inline Heap* getAllocatableHeap() { return mAllocatableHeap; }
    



	// _00 = VTBL
    Heap* mContainHeap;        		// _04
    OSThread* mOSThread;       		// _08
    OSMessageQueue mMesgQueue; 		// _0C
    OSMessage* mMesgBuffer;    		// _2C
    int mMesgNum;              		// _30
    void* mStackMemory;        		// _34
    u32 mStackSize;            		// _38
	Heap *mAllocatableHeap; 		// _3C
    // TODO from the usage in eggThread this really looks like
    // it's a stashed heap that's restored when switching threads
    Heap *mCurrentHeap; 			// _40


    NW4R_UT_LIST_LINK_DECL(); 		// _44

private:
    static nw4r::ut::List sThreadList;
};

} // namespace EGG

#endif