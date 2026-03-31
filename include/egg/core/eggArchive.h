#ifndef _EGG_CORE_ALLOCATOR_H
#define _EGG_CORE_ALLOCATOR_H

#include <egg/egg_types.h>
#include <egg/core/eggDisposer.h>
#include <nw4r/ut.h>
#include <RevoSDK/arc.h>

namespace EGG {

class Heap;

class Archive : public Disposer {
public:
    enum MountType { MOUNT_TYPE_NONE, MOUNT_TYPE_MEM, MOUNT_TYPE_FST };

    class FileInfo {
        friend class Archive;

    public:
        FileInfo() : mStartOffset(0), mLength(0) {}

        u32 getStartOffset() const {
            return mStartOffset;
        }
        u32 getLength() const {
            return mLength;
        }

    private:
        u32 mStartOffset; // _00
        u32 mLength;      // _04
    };

public:
    static Archive* mount(void* pArcBinary, Heap* pHeap, int align);
    static Archive* findArchive(void* pArcBinary);

    Archive();
    virtual ~Archive(); // _08

    void* getFile(const char* pPath, FileInfo* pInfo);

private:
    bool initHandle(void* pArcBinary);

    static void appendList(Archive* pArchive);
    static void removeList(Archive* pArchive);

private:
	// _00     = VTBL
	// _00-_10 = Disposer
    MountType mMountType;	   // _10
    u32 mRefCount;       	   // _14
    ARCHandle mHandle;    	   // _18

public:
    NW4R_UT_LIST_LINK_DECL();  // _34

private:
    static nw4r::ut::List sArchiveList;
    static bool sIsArchiveListInitialized;
};

} // namespace EGG

#endif
