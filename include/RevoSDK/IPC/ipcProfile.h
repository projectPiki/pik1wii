#ifndef REVOSDK_IPC_PROFILE_H
#define REVOSDK_IPC_PROFILE_H
#include <types.h>
#include <RevoSDK/IPC/ipcclt.h>
#ifdef __cplusplus
extern "C" {
#endif

void IPCiProfInit(void);
void IPCiProfQueueReq(IPCRequestEx* req, s32 fd);
void IPCiProfAck(void);
void IPCiProfReply(IPCRequestEx* req, s32 fd);

#ifdef __cplusplus
}
#endif
#endif