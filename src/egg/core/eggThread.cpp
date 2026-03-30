#include <egg/core/eggThread.h>

#include <nw4r/ut.h>

#include "RevoSDK/os.h"

namespace EGG {

nw4r::ut::List Thread::sThreadList;

Thread* Thread::findThread(OSThread* pOSThread) {
    NW4R_UT_LIST_FOREACH (Thread, it, sThreadList, {
        if (it->mOSThread == pOSThread) {
            return it;
        }
    })

    return nullptr;
}


} // namespace EGG