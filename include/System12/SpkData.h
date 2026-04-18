#ifndef SYSTEM12_SPKDATA_H
#define SYSTEM12_SPKDATA_H

#include "System12/SpkTable.h"
#include "System12/SpkWave.h"
#include "egg/core/eggArchive.h"

class SpkData {
public:
    SpkData(EGG::Archive*);
    void loadTable(const char*);
    void loadWave(const char*);
    BOOL isValid(void) const;

    inline SpkTable& getTableMgr(void) { return mTableMgr; }
    inline SpkWave& getWaveMgr(void) { return mWaveMgr; }

    SpkTable mTableMgr;			// _00
    SpkWave mWaveMgr;			// _10
    EGG::Archive* mResArc;		// _14
};

#endif