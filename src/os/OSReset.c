#include "RevoSDK/hw_regs.h"
#include "RevoSDK/os.h"
#include <stddef.h>

static OSShutdownFunctionQueue ShutdownFunctionQueue;
static u32 bootThisDol = 0;
volatile BOOL __OSIsReturnToIdle = FALSE;

static void KillThreads(void);

/**
 * @TODO: Documentation
 */
void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info)
{
	OSShutdownFunctionInfo* tmp;
	OSShutdownFunctionInfo* iter;

	for (iter = ShutdownFunctionQueue.head; iter && iter->priority <= info->priority; iter = iter->next) {
		;
	}

	if (iter == NULL) {
		tmp = ShutdownFunctionQueue.tail;
		if (tmp == NULL) {
			ShutdownFunctionQueue.head = info;
		} else {
			tmp->next = info;
		}
		info->prev              = tmp;
		info->next              = NULL;
		ShutdownFunctionQueue.tail = info;
		return;
	}

	info->next = iter;
	tmp        = iter->prev;
	iter->prev = info;
	info->prev = tmp;
	if (tmp == NULL) {
		ShutdownFunctionQueue.head = info;
		return;
	}
	tmp->next = info;
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000038
 */
void OSUnregisterShutdownFunction(OSShutdownFunctionInfo*)
{
	// UNUSED FUNCTION
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 00008C
 */
int __OSCallShutdownFunctions(BOOL final, u32 event)
{
	OSShutdownFunctionInfo* info;
    int err;
    u32 priority;

    priority = 0;
    err = 0;

    for (info = ShutdownFunctionQueue.head; info != 0;) {
        if (err != 0 && priority != info->priority)
            break;
        err |= !info->func(final, event);
        priority = info->priority;
        info = info->next;
    }

    err |= !__OSSyncSram();
    return err ? FALSE : TRUE;
	// UNUSED FUNCTION
}

void __OSShutdownDevices(u32 event) {
    BOOL rc, disableRecalibration, doRecal;

    switch(event) {
      case 0:
      case 5:
      case 6:
        doRecal = FALSE;
        break;
      case 2:
      case 3:
      case 4:
      case 1:
      default:
        doRecal = TRUE;
        break;
    }

    __OSStopAudioSystem();

    if (!doRecal) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }

    while (!__OSCallShutdownFunctions(FALSE, event));

    while (!__OSSyncSram());

    OSDisableInterrupts();
    rc = __OSCallShutdownFunctions(TRUE, event);
    OSAssertLine(491, rc);
    LCDisable();

    if (!doRecal) {
        __PADDisableRecalibration(disableRecalibration);
    }

    KillThreads();
}

u8 __OSGetDiscState(u8 last) {
    u32 flags;

    if (__DVDGetCoverStatus() != 2) {
        return 3;
    } else {
        if ((last == 1) && (__OSGetRTCFlags(&flags) && !flags)) {
            return 1;
        } else {
            return 2;
        }
    }
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000068
 */
static void KillThreads(void)
{
	OSThread* thread;
	OSThread* next;

	for (thread = __OSActiveThreadQueue.head; thread; thread = next) {
		next = thread->linkActive.next;
		switch (thread->state) {
		case 1:
		case 4:
		{
			OSCancelThread(thread);
			break;
		}
		}
	}
	// UNUSED FUNCTION
}

/**
 * @TODO: Documentation
 */
void __OSDoHotReset(s32 code)
{
	OSDisableInterrupts();
	__VIRegs[VI_DISP_CONFIG] = 0;
	ICFlashInvalidate();
	Reset(code * 8);
}

/**
 * @TODO: Documentation
 */
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu)
{
    OSErrorLine(1130,"OSResetSystem() is obsoleted. It doesn't work any longer.\n");
}