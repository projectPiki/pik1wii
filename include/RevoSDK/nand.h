#ifndef REVOSDK_NAND_H
#define REVOSDK_NAND_H

#include <types.h>

#include <RevoSDK/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_BANNER_TITLE_MAX 32
#define NAND_BANNER_ICON_MAX_FRAME 8

// Forward declarations
typedef struct NANDCommandBlock NANDCommandBlock;

typedef enum {
    NAND_RESULT_FATAL_ERROR = -128,
    NAND_RESULT_UNKNOWN = -64,

    NAND_RESULT_MAXDEPTH = -16,
    NAND_RESULT_AUTHENTICATION,
    NAND_RESULT_OPENFD,
    NAND_RESULT_NOTEMPTY,
    NAND_RESULT_NOEXISTS,
    NAND_RESULT_MAXFILES,
    NAND_RESULT_MAXFD,
    NAND_RESULT_MAXBLOCKS,
    NAND_RESULT_INVALID,

    NAND_RESULT_EXISTS = -6,
    NAND_RESULT_ECC_CRIT,
    NAND_RESULT_CORRUPT,
    NAND_RESULT_BUSY,
    NAND_RESULT_ALLOC_FAILED,
    NAND_RESULT_ACCESS,

    NAND_RESULT_OK,
} NANDResult;

typedef enum {
    NAND_SEEK_BEG,
    NAND_SEEK_CUR,
    NAND_SEEK_END,
} NANDSeekMode;

typedef enum {
    NAND_ACCESS_NONE,
    NAND_ACCESS_READ,
    NAND_ACCESS_WRITE,
    NAND_ACCESS_RW
} NANDAccessType;

typedef enum {
    NAND_FILE_TYPE_NONE,
    NAND_FILE_TYPE_FILE,
    NAND_FILE_TYPE_DIR,
} NANDFileType;

typedef enum {
    NAND_CHECK_TOO_MANY_APP_BLOCKS = (1 << 0),
    NAND_CHECK_TOO_MANY_APP_FILES = (1 << 1),
    NAND_CHECK_TOO_MANY_USER_BLOCKS = (1 << 2),
    NAND_CHECK_TOO_MANY_USER_FILES = (1 << 3),
} NANDCheckFlags;

typedef enum {
    // Read/write by owner
    NAND_PERM_RUSR = (NAND_ACCESS_READ << 4),
    NAND_PERM_WUSR = (NAND_ACCESS_WRITE << 4),
    // Read/write by group
    NAND_PERM_RGRP = (NAND_ACCESS_READ << 2),
    NAND_PERM_WGRP = (NAND_ACCESS_WRITE << 2),
    // Read/write by other
    NAND_PERM_ROTH = (NAND_ACCESS_READ << 0),
    NAND_PERM_WOTH = (NAND_ACCESS_WRITE << 0),
    // Read/write by all
    NAND_PERM_RALL = NAND_PERM_RUSR | NAND_PERM_RGRP | NAND_PERM_ROTH,
    NAND_PERM_WALL = NAND_PERM_WUSR | NAND_PERM_WGRP | NAND_PERM_WOTH,
    NAND_PERM_RWALL = NAND_PERM_RALL | NAND_PERM_WALL
} NANDPermission;

typedef void (*NANDAsyncCallback)(s32 result, NANDCommandBlock* block);

typedef struct NANDStatus {
    u32 ownerId; // _00
    u16 groupId; // _04
    u8 attr;     // _06
    u8 perm;     // _07
} NANDStatus;

typedef struct NANDFileInfo {
    s32 fd;                     // _00
    s32 tempFd;                 // _04
    char openPath[FS_MAX_PATH]; // _08
    char tempPath[FS_MAX_PATH]; // _48
    u8 access;                  // _88
    u8 stage;                   // _89
    u8 mark;                    // _8A
} NANDFileInfo;

typedef struct NANDCommandBlock {
    void* userData;             // _00
    NANDAsyncCallback callback; // _04
    NANDFileInfo* info;         // _08
    void* bytes;                // _0C
    void* inodes;               // _10
    NANDStatus* status;         // _14
    u32 ownerId;                // _18
    u16 groupId;                // _1C
    u8 nextStage;               // _1E
    u32 attr;                   // _20
    u32 ownerPerm;              // _24
    u32 groupPerm;              // _28
    u32 otherPerm;              // _2C
    u32 dirFileCount;           // _30
    char path[FS_MAX_PATH];     // _34
    u32* length;                // _74
    u32* position;              // _78
    s32 state;                  // _7C
    void* buffer;               // _80
    u32 bufferSize;             // _84
    u8* type;                   // _88
    u32 uniqueNo;               // _8C
    u32 reqBlocks;              // _90
    u32 reqInodes;              // _94
    u32* answer;                // _98
    u32 homeBlocks;             // _9C
    u32 homeInodes;             // _A0
    u32 userBlocks;             // _A4
    u32 userInodes;             // _A8
    u32 workBlocks;             // _AC
    u32 workInodes;             // _B0
    const char** dir;           // _B4
    BOOL simpleFlag;            // _B8
} NANDCommandBlock;

typedef struct NANDBanner {
    u32 magic;                                          // _00
    u32 flags;                                          // _04
    u16 iconSpeed;                                      // _08
    u8 reserved[0x20 - 0xA];                            // _0A
    wchar_t title[NAND_BANNER_TITLE_MAX];               // _20
    wchar_t subtitle[NAND_BANNER_TITLE_MAX];            // _60
    u8 bannerTexture[0x6000];                           // _A0
    u8 iconTexture[0x1200][NAND_BANNER_ICON_MAX_FRAME]; // _60A0
} NANDBanner;

typedef void (*NANDLoggingCallback)(BOOL);

s32 NANDCreate(const char* path, u8 perm, u8 attr);
s32 NANDPrivateCreate(const char* path, u8 perm, u8 attr);
s32 NANDPrivateCreateAsync(const char* path, u8 perm, u8 attr,
                           NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDDelete(const char* path);
s32 NANDPrivateDelete(const char* path);
s32 NANDPrivateDeleteAsync(const char* path, NANDAsyncCallback callback,
                           NANDCommandBlock* block);

s32 NANDRead(NANDFileInfo* info, void* buf, u32 len);
s32 NANDReadAsync(NANDFileInfo* info, void* buf, u32 len,
                  NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDWrite(NANDFileInfo* info, const void* buf, u32 len);
s32 NANDWriteAsync(NANDFileInfo* info, const void* buf, u32 len,
                   NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDSeek(NANDFileInfo* info, s32 offset, NANDSeekMode whence);
s32 NANDSeekAsync(NANDFileInfo* info, s32 offset, NANDSeekMode whence,
                  NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDPrivateCreateDir(const char* path, u8 perm, u8 attr);
s32 NANDPrivateCreateDirAsync(const char* path, u8 perm, u8 attr,
                              NANDAsyncCallback callback,
                              NANDCommandBlock* block);

s32 NANDMove(const char* from, const char* to);

s32 NANDGetLength(NANDFileInfo* info, u32* length);
s32 NANDGetLengthAsync(NANDFileInfo* info, u32* lengthOut,
                       NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDGetStatus(const char* path, NANDStatus* status);
s32 NANDPrivateGetStatusAsync(const char* path, NANDStatus* status,
                              NANDAsyncCallback callback,
                              NANDCommandBlock* block);

void NANDSetUserData(NANDCommandBlock* block, void* data);
void* NANDGetUserData(NANDCommandBlock* block);

s32 NANDOpen(const char* path, NANDFileInfo* info, u8 mode);
s32 NANDPrivateOpen(const char* path, NANDFileInfo* info, u8 mode);
s32 NANDOpenAsync(const char* path, NANDFileInfo* info, u8 mode,
                  NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDPrivateOpenAsync(const char* path, NANDFileInfo* info, u8 mode,
                         NANDAsyncCallback callback, NANDCommandBlock* block);

s32 NANDClose(NANDFileInfo* info);
s32 NANDCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback,
                   NANDCommandBlock* block);

s32 NANDPrivateSafeOpenAsync(const char* path, NANDFileInfo* info, u8 access,
                             void* buffer, u32 bufferSize,
                             NANDAsyncCallback callback,
                             NANDCommandBlock* block);
s32 NANDSafeCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback,
                       NANDCommandBlock* block);


s32 NANDCheck(u32 neededBlocks, u32 neededFiles, u32* answer);

void nandRemoveTailToken(char* newp, const char* oldp);
void nandGetHeadToken(char* head, char* rest, const char* path);
void nandGetRelativeName(char* name, const char* path);
void nandConvertPath(char* abs, const char* dir, const char* rel);
BOOL nandIsRelativePath(const char* path);
BOOL nandIsPrivatePath(const char* path);
BOOL nandIsUnderPrivatePath(const char* path);
BOOL nandIsInitialized(void);
void nandReportErrorCode(s32 result) DECOMP_DONT_INLINE;
s32 nandConvertErrorCode(s32 result);
void nandGenerateAbsPath(char* abs, const char* rel);
void nandGetParentDirectory(char* dir, const char* path);
s32 NANDInit(void);
s32 NANDGetCurrentDir(char* out);
s32 NANDGetHomeDir(char* out);
void nandCallback(s32 result, void* arg);
s32 NANDGetType(const char* path, u8* type);
s32 NANDPrivateGetTypeAsync(const char* path, u8* type,
                            NANDAsyncCallback callback,
                            NANDCommandBlock* block);
const char* nandGetHomeDir(void);
void NANDInitBanner(NANDBanner* banner, u32 flags, const wchar_t* title,
                    const wchar_t* subtitle);

BOOL NANDLoggingAddMessageAsync(NANDLoggingCallback, const char *, ...);


#ifdef __cplusplus
}
#endif

#endif