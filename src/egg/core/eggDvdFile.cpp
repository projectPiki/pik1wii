#include <egg/core/eggDvdFile.h>

namespace EGG {

nw4r::ut::List DvdFile::sDvdList;
bool DvdFile::sIsInitialized = false;

void DvdFile::initialize()
{
	if (sIsInitialized) {
		return;
	}

	NW4R_UT_LIST_INIT(sDvdList, DvdFile);
	sIsInitialized = true;
}
DvdFile::DvdFile()
{
	initiate();
}

DvdFile::~DvdFile()
{
	close();
}

void DvdFile::initiate()
{
	mAsyncContext.pFile = this;

	OSInitMutex(&mSyncMutex);
	OSInitMutex(&mAsyncMutex);

	OSInitMessageQueue(&mSyncQueue, mSyncBuffer, ARRAY_SIZE(mSyncBuffer));
	OSInitMessageQueue(&mAsyncQueue, mAsyncBuffer, ARRAY_SIZE(mAsyncBuffer));

	mSyncThread  = nullptr;
	mAsyncThread = nullptr;
}

bool DvdFile::open(s32 entryNum)
{
	if (!mIsOpen && entryNum != -1) {
		mIsOpen = DVDFastOpen(entryNum, &mAsyncContext.fileInfo);

		if (mIsOpen) {
			nw4r::ut::List_Append(&sDvdList, this);
			(void)getStatus();
		}
	}

	return mIsOpen;
}

bool DvdFile::open(const char* pPath)
{
	s32 entryNum = DVDConvertPathToEntrynum(pPath);
	return open(entryNum);
}

bool DvdFile::open(const char* pPath, void* pMultiHandle)
{
	return open(pPath);
}

void DvdFile::close()
{
	if (mIsOpen && DVDClose(&mAsyncContext.fileInfo)) {
		mIsOpen = false;
		nw4r::ut::List_Remove(&sDvdList, this);
	}
}

s32 DvdFile::readData(void* pDst, s32 size, s32 offset)
{
	OSLockMutex(&mSyncMutex);

	if (mSyncThread != nullptr) {
		OSUnlockMutex(&mSyncMutex);
		return -1;
	}

	mSyncThread = OSGetCurrentThread();

	s32 result = -1;
	if (DVDReadAsyncPrio(&mAsyncContext.fileInfo, pDst, size, offset, DvdFile::doneProcess, DVD_PRIO_MEDIUM)) {

		result = sync();
	}

	mSyncThread = nullptr;

	OSUnlockMutex(&mSyncMutex);
	return result;
}

s32 DvdFile::writeData(const void* pSrc, s32 size, s32 offset)
{
	return -1;
}

s32 DvdFile::sync()
{
	OSLockMutex(&mSyncMutex);

	OSMessage buffer[1];
	OSReceiveMessage(&mSyncQueue, buffer, ARRAY_SIZE(buffer));

	mSyncThread = nullptr;
	OSUnlockMutex(&mSyncMutex);

	return reinterpret_cast<s32>(buffer[0]);
}

void DvdFile::doneProcess(s32 result, DVDFileInfo* pFileInfo)
{
	AsyncContext* pContext = reinterpret_cast<AsyncContext*>(pFileInfo);

	OSSendMessage(&pContext->pFile->mSyncQueue, reinterpret_cast<OSMessage>(result), 0);
}

} // namespace EGG
