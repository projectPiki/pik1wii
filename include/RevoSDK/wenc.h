#ifndef REVOSDK_WENC_H
#define REVOSDK_WENC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

typedef enum {
    WENC_FLAG_USER_INFO = (1 << 0),
} WENCFlag;

typedef struct WENCInfo {
    u8 data[32];
} WENCInfo;

s32 WENCGetEncodeData(WENCInfo*, u32, const s16*, s32, u8*);

#ifdef __cplusplus
}
#endif

#endif
