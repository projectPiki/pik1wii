#ifndef REVOSDK_IPC_H
#define REVOSDK_IPC_H

#include <types.h>

#include "RevoSDK/IPC/ipcProfile.h"
#include "RevoSDK/IPC/ipcclt.h"
#include "RevoSDK/IPC/memory.h"

extern u32 IPCReadReg(u32);

void IPCInit(void);
s32 IPCCltInit(void);

extern void* IPCGetBufferHi(void);
extern void* IPCGetBufferLo(void);
extern void IPCSetBufferLo(void*);

#endif