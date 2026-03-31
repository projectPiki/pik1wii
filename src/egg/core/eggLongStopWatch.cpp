#include <egg/core/eggLongStopWatch.h>

namespace EGG {

void LongStopWatch::start() {
    if (!isRunning()) {
        mBaseTime += OSGetTime() - mStopTime;
        mStopTime = 0;
    }
}

void LongStopWatch::reset() {
    mBaseTime = OSGetTime();
    if (!isRunning()) {
        mStopTime = OSGetTime();
    }
}

LongStopWatch::LongStopWatch() {
    stop();
    reset();
}

} // namespace EGG