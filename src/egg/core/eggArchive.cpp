#include <egg/core/eggArchive.h>
#include <egg/core/eggHeap.h>
#include <string.h>

namespace EGG {

nw4r::ut::List Archive::sArchiveList;
bool Archive::sIsArchiveListInitialized = false;

Archive::Archive() {
    mRefCount = 1;
    mMountType = MOUNT_TYPE_NONE;
    memset(&mHandle, 0, sizeof(ARCHandle));
    appendList(this);
}

Archive::~Archive() {
    removeList(this);
}

bool Archive::initHandle(void* arcBinary) {

    return ARCInitHandle(arcBinary, &mHandle);
}

Archive* Archive::findArchive(void* pArcBinary) {
    Archive* pArchive = nullptr;

    if (sIsArchiveListInitialized) {
        NW4R_UT_LIST_FOREACH (Archive, it, sArchiveList, {
            if (it->mHandle.archiveStartAddr == pArcBinary) {
                pArchive = it;
                break;
            }
        })
    }

    return pArchive;
}

void Archive::appendList(Archive* pArchive) {
    if (!sIsArchiveListInitialized) {
        NW4R_UT_LIST_INIT(sArchiveList, Archive);
        sIsArchiveListInitialized = true;
    }

    nw4r::ut::List_Append(&sArchiveList, pArchive);
}

void Archive::removeList(Archive* pArchive) {
    nw4r::ut::List_Remove(&sArchiveList, pArchive);
}

Archive* Archive::mount(void* arcBinary, Heap* pHeap, int align) {

    Archive* archive = findArchive(arcBinary);

    if (archive == nullptr) {
        archive = new (pHeap, align) Archive();


        bool success = archive->initHandle(arcBinary);

        if (success) {
            archive->mMountType = MOUNT_TYPE_MEM;
        } else {
            archive->mMountType = MOUNT_TYPE_NONE;
        }

        if (!success) {

            delete archive;
            archive = nullptr;
        }

    } else {
        archive->mRefCount++;
    }

    return archive;
}

void* Archive::getFile(const char* pPath, FileInfo* pInfo) {
    void* pStartAddr = nullptr;

    ARCFileInfo info;
    bool success = ARCOpen(&mHandle, pPath, &info);

    if (success) {
        if (mMountType == MOUNT_TYPE_MEM) {
            pStartAddr = ARCGetStartAddrInMem(&info);
        }

        if (pInfo != nullptr) {
            pInfo->mStartOffset = ARCGetStartOffset(&info);
            pInfo->mLength = ARCGetLength(&info);
        }
    }

    ARCClose(&info);
    return pStartAddr;
}

} // namespace EGG