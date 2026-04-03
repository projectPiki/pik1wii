#include <RevoSDK/nand.h>
#include <RevoSDK/fs.h>

s32 NANDSecretGetUsage(const char* path, s32* fsBlock, s32* inode) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }
    else {
        char absPath[64] = "";
        nandGenerateAbsPath(absPath, path);
        return nandConvertErrorCode(ISFS_GetUsage(absPath, fsBlock, inode));
    }
}