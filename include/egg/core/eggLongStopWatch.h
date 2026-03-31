#ifndef EGG_CORE_LONGSTOPWATCH_H
#define EGG_CORE_LONGSTOPWATCH_H

#include <egg/egg_types.h>

#include <RevoSDK/os.h>

namespace EGG {

class LongStopWatch {
public:
    LongStopWatch();

    void start();
    void reset();

    void stop() {
        if (isRunning()) {
            mStopTime = OSGetTime();
        }
    }

    bool isRunning() {
        return mStopTime == 0;
    }

    u64 getTime() {
        u64 time = isRunning() ? OSGetTime() : mStopTime;
        return time - mBaseTime;
    }

private:
    u64 mStopTime; 	// _00
    u64 mBaseTime; 	// _08
};

} // namespace EGG

#endif
