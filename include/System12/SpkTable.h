#ifndef SYSTEM12_SPKTABLE_H
#define SYSTEM12_SPKTABLE_H

#include "egg/prim/eggAssert.h"
#include <types.h>
#include <RevoSDK/wpad.h>

struct SpkTableParams {
    u16 mWaveNum;	// _00
    u8 _02;			// _02
    u8 mVolume;		// _03
    u16 mMsec;		// _04
    u8 _06[2];		// _06
};

class SpkTable {
public:
    SpkTable(void);
    void setResource(void* res);

    inline s32 getName(s32 num) {
        EGG_ASSERT(0x35, num >= 0);
        EGG_ASSERT(0x36, num < mNumOfSound);
        return *(mDataOffsets + num);
    }
    inline s32 getNumOfSound() const { return mNumOfSound; }
    inline bool isValid(void) const { return mIsInitialized; }
    inline SpkTableParams* getParams(s32 num) {
        EGG_ASSERT(46, num >= 0);
        EGG_ASSERT(47, num < mNumOfSound);
        return (SpkTableParams*)mEntryOffset + num;
    }


    bool mIsInitialized;	// _00
    s32 mNumOfSound;		// _04
    u32 mEntryOffset;		// _08
    s32* mDataOffsets;		// _0C
};

#endif