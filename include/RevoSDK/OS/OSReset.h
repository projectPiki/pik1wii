#ifndef _REVOSDK_OS_OSRESET_H
#define _REVOSDK_OS_OSRESET_H

#include "RevoSDK/OS/OSContext.h"
#include "types.h"

BEGIN_SCOPE_EXTERN_C

typedef enum {
    OS_SD_EVENT_FATAL,
    OS_SD_EVENT_1,
    OS_SD_EVENT_SHUTDOWN,
    OS_SD_EVENT_3,
    OS_SD_EVENT_RESTART,
    OS_SD_EVENT_RETURN_TO_MENU,
    OS_SD_EVENT_LAUNCH_APP,
} OSShutdownEvent;

/////////// SHUTDOWN INFO ///////////
typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;

// Shutdown function type.
typedef BOOL (*OSShutdownFunction)(BOOL final, u32 event);

// Reset callback type.
typedef void (*OSResetCallback)(void);

// Struct for storing shutdown function information.
struct OSShutdownFunctionInfo {
	OSShutdownFunction func;        // _00
	u32 priority;              		// _04
	OSShutdownFunctionInfo* next;   // _08
	OSShutdownFunctionInfo* prev;   // _0C
};


// Queue struct for ShutdownFunctionInfos.
typedef struct OSShutdownFunctionQueue {
	OSShutdownFunctionInfo* head;
	OSShutdownFunctionInfo* tail;
} OSShutdownFunctionQueue;

//////////////////////////////////

//////// SHUTDOWN FUNCTIONS /////////
// Basic shutdown functions.
void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info);
int __OSCallShutdownFunctions(BOOL final, u32 event);
void __OSShutdownDevices(u32 event);
u8 __OSGetDiscState(u8);
void OSShutdownSystem(void);
void OSReturnToMenu(void);
u32 OSGetResetCode(void);
void OSResetSystem(int reset, u32 code, BOOL doForceMenu);

// Reset switch functions.
BOOL OSGetResetButtonState();
BOOL OSGetResetSwitchState();

//////////////////////////////////

///////// RESET DEFINES //////////
// Reset codes.
#define OS_RESETCODE_RESTART 0x80000000
#define OS_RESETCODE_SYSTEM  0x40000000

#define OS_RESETCODE_EXEC      0xC0000000
#define OS_RESETCODE_NETCONFIG 0xC0010000

#define OS_RESET_TIMEOUT OSMillisecondsToTicks(1000)

#define OS_RESET_RESTART  0
#define OS_RESET_HOTRESET 1
#define OS_RESET_SHUTDOWN 2

// Reset priorities.
#define OS_RESET_PRIO_SO    110
#define OS_RESET_PRIO_IP    111
#define OS_RESET_PRIO_CARD  127
#define OS_RESET_PRIO_MEM   127
#define OS_RESET_PRIO_PAD   127
#define OS_RESET_PRIO_GX    127
#define OS_RESET_PRIO_ALARM 0xFFFFFFFF

extern BOOL __OSIsGcam;

//////////////////////////////////

END_SCOPE_EXTERN_C

#endif
