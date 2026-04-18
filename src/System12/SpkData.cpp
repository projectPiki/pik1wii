#include "System12/SpkData.h"
#include "egg/prim/eggAssert.h"

SpkData::SpkData(EGG::Archive* resArc) {
    mResArc = resArc;
    EGG_ASSERT(20, resArc);
}

void SpkData::loadTable(const char* file) {
    EGG_ASSERT(32, mResArc);

    void* res = mResArc->getFile(file, nullptr);
    EGG_ASSERT(36, res);

    mTableMgr.setResource(res);
}

void SpkData::loadWave(const char* file) {
    EGG_ASSERT(50, mResArc);

    void* res = mResArc->getFile(file, nullptr);
    EGG_ASSERT(54, res);

    mWaveMgr.setResource(res);
}

BOOL SpkData::isValid(void) const {
    return mTableMgr.isValid() && mWaveMgr.isValid();
}
/*
static void dummy(SpkWave* wave, SpkTable* table) {
    if (wave->isValid() && table->isValid()) {
        table->getParams(0);
        table->getNumOfSound();
    }
}*/