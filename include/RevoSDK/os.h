#ifndef _REVOSDK_OS_H
#define _REVOSDK_OS_H

#include "types.h"

#include "RevoSDK/OS/OSAlarm.h"
#include "RevoSDK/OS/OSAlloc.h"
#include "RevoSDK/OS/OSArena.h"
#include "RevoSDK/OS/OSBootInfo.h"
#include "RevoSDK/OS/OSCache.h"
#include "RevoSDK/OS/OSContext.h"
#include "RevoSDK/OS/OSError.h"
#include "RevoSDK/OS/OSException.h"
#include "RevoSDK/OS/OSExi.h"
#include "RevoSDK/OS/OSExpansion.h"
#include "RevoSDK/OS/OSFont.h"
#include "RevoSDK/OS/OSInterrupt.h"
#include "RevoSDK/OS/OSMemory.h"
#include "RevoSDK/OS/OSMessage.h"
#include "RevoSDK/OS/OSModule.h"
#include "RevoSDK/OS/OSMutex.h"
#include "RevoSDK/OS/OSReset.h"
#include "RevoSDK/OS/OSRtc.h"
#include "RevoSDK/OS/OSSerial.h"
#include "RevoSDK/OS/OSThread.h"
#include "RevoSDK/OS/OSTime.h"
#include "RevoSDK/OS/OSUtil.h"

#include "RevoSDK/OS/OSFastCast.h"

BEGIN_SCOPE_EXTERN_C

///////// OS FUNCTIONS ///////////
// Initialisers.
extern void __OSPSInit();
extern void __OSFPRInit();
extern void __OSCacheInit();
extern void __OSContextInit();
extern void __OSInterruptInit();
extern void __OSInitSystemCall();
extern void __OSModuleInit();
extern void __OSInitAudioSystem();
extern void __OSStopAudioSystem();
extern void __OSInitMemoryProtection();

void OSInit();

// Other OS functions.
#define OS_CONSOLE_RETAIL4     0x00000004
#define OS_CONSOLE_RETAIL3     0x00000003
#define OS_CONSOLE_RETAIL2     0x00000002
#define OS_CONSOLE_RETAIL1     0x00000001
#define OS_CONSOLE_RETAIL      0x00000000
#define OS_CONSOLE_DEVHW4      0x10000007
#define OS_CONSOLE_DEVHW3      0x10000006
#define OS_CONSOLE_DEVHW2      0x10000005
#define OS_CONSOLE_DEVHW1      0x10000004
#define OS_CONSOLE_MINNOW      0x10000003
#define OS_CONSOLE_ARTHUR      0x10000002
#define OS_CONSOLE_PC_EMULATOR 0x10000001
#define OS_CONSOLE_EMULATOR    0x10000000
#define OS_CONSOLE_DEVELOPMENT 0x10000000
#define OS_CONSOLE_DEVKIT      0x10000000
#define OS_CONSOLE_TDEVKIT     0x20000000

u32 OSGetConsoleType();

//////////////////////////////////

// extern things.
extern u8 _stack_addr[];
extern u8 _stack_end[];

extern BOOL __OSInIPL;
extern OSTime __OSStartTime;

//////////////////////////////////

END_SCOPE_EXTERN_C

#endif
