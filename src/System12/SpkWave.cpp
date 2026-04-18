#include "System12/SpkWave.h"

#include "egg/prim/eggAssert.h"
#include <RevoSDK/os.h>
#include <types.h>

SpkWave::SpkWave(void) {
    mWaveData = nullptr;
}

void SpkWave::setResource(void* res) {
    EGG_ASSERT(32, res);

	BOOL status = OSDisableInterrupts();
    mWaveData = res;
	OSRestoreInterrupts(status);
}

s32 SpkWave::getNumOfWaves(void) const {
    if (mWaveData != nullptr) {
        return ((s32*)mWaveData)[1];
    }

    return 0;
}

s32 SpkWave::getWaveSize(s32 num) const {
    EGG_ASSERT(60, num >= 0);
    if (!mWaveData) {
        return 0;
    }
    EGG_ASSERT(63, num < getNumOfWaves());

    WaveData* data = getWaveData(num);
    EGG_ASSERT(66, data);
    return data->size;
}

u32 SpkWave::getLoopStartPos(s32 num) const {
    EGG_ASSERT(79, num >= 0);
    if (!mWaveData) {
        return 0;
    }
    EGG_ASSERT(82, num < getNumOfWaves());

    WaveData* data = getWaveData(num);
    EGG_ASSERT(85, data);
    return data->loopStartPos;
}

u32 SpkWave::getLoopEndPos(s32 num) const {
    EGG_ASSERT(98, num >= 0);
    if (!mWaveData) {
        return 0;
    }
    EGG_ASSERT(101, num < getNumOfWaves());

    WaveData* data = getWaveData(num);
    EGG_ASSERT(104, data);
    return data->loopEndPos;
}

s16* SpkWave::getWave(s32 num) const {
    EGG_ASSERT(117, num >= 0);
    if (!mWaveData) {
        return 0;
    }
    EGG_ASSERT(120, num < getNumOfWaves());

    WaveData* data = getWaveData(num);
    EGG_ASSERT(123, data);
    return data->wave;
}

WaveData* SpkWave::getWaveData(s32 num) const {
    EGG_ASSERT(141, num >= 0);
    EGG_ASSERT(142, mWaveData);
    EGG_ASSERT(143, num < getNumOfWaves());

    WaveData* data = (WaveData*)((u32)mWaveData + *(u32*)((u32)mWaveData + num * 4 + 8));
    return data;
}