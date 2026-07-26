#include "RevoSDK/dsp.h"
#include "RevoSDK/os.h"
#include "jaudio/dspproc.h"
#include "jaudio/dummyprobe.h"

static vu8 DSP_prior_yield;
static u32 sync_stack[5];
DSPTaskInfo* DSP_prior_task;
static u8 lbl_8049E101;
static vu8 DspRunningStatus;

static void Dsp_Update_Request();

BEGIN_SCOPE_EXTERN_C

/**
 * @TODO: Documentation
 */
void __DSPHandler(__OSInterrupt interrupt, OSContext* context)
{
	OSContext funcContext;
	int stack[3];

	__DSPRegs[5] = ((u16)(__DSPRegs[5]) & ~0x28) | 0x80;

	OSClearContext(&funcContext);
	OSSetCurrentContext(&funcContext);

	if (DSP_prior_yield == 1 || DSP_prior_yield == 0) {
		__DSP_curr_task = DSP_prior_task;
	}

	while (DSPCheckMailFromDSP() == 0)
		;
	u32 mail = DSPReadMailFromDSP();

	if ((__DSP_curr_task->flags & 2) && mail == 0xDCD10002) {
		mail = 0xDCD10003;
	}

	switch (mail) {
	case 0xDCD10000:
	{
		__DSP_curr_task->state = 1;
		if (__DSP_curr_task == DSP_prior_task) {
			DSP_prior_yield = 1;
		}
		if (__DSP_curr_task->init_cb != nullptr) {
			__DSP_curr_task->init_cb(__DSP_curr_task);
		}
		break;
	}
	case 0xDCD10001:
	{
		__DSP_curr_task->state = 1;
		if (__DSP_curr_task == DSP_prior_task) {
			DSP_prior_yield = 1;
			Dsp_Update_Request();
		}
		if (__DSP_curr_task->res_cb != nullptr) {
			__DSP_curr_task->res_cb(__DSP_curr_task);
		}
		OSReport("Audio Resumed\n");
		break;
	}
	case 0xDCD10002:
	{
		OSReport("Yield Handler\n");
		DSPSendMailToDSP(0xCDD10001);
		while (DSPCheckMailToDSP() != 0)
			;
		__DSP_curr_task->state = 2;
		if (__DSP_curr_task->next == nullptr && lbl_8049E101) {
			__DSP_exec_task(__DSP_curr_task, DSP_prior_task);
			lbl_8049E101    = 0;
			__DSP_curr_task = DSP_prior_task;
		} else {
			__DSP_exec_task(__DSP_curr_task, __DSP_curr_task->next);
			__DSP_curr_task = __DSP_curr_task->next;
		}
		break;
	}
	case 0xDCD10003:
	{
		OSReport("Done DSP Task  %x \n", (u32)__DSP_curr_task);
		if (__DSP_curr_task->done_cb) {
			__DSP_curr_task->done_cb(__DSP_curr_task);
		}
		DSPSendMailToDSP(0xCDD10001);
		while (DSPCheckMailToDSP() != 0)
			;
		__DSP_curr_task->state = 3;
		if (__DSP_curr_task->next == nullptr) {
			__DSP_exec_task(nullptr, DSP_prior_task);
			__DSP_remove_task(__DSP_curr_task);
			__DSP_curr_task = DSP_prior_task;
		} else {
			__DSP_exec_task(nullptr, __DSP_curr_task->next);
			__DSP_curr_task = __DSP_curr_task->next;
			__DSP_remove_task(__DSP_curr_task->prev);
		}
		break;
	}
	case 0xDCD10004:
	{
		if (__DSP_curr_task->req_cb != nullptr) {
			__DSP_curr_task->req_cb(__DSP_curr_task);
		}
		break;
	}
	case 0xDCD10005:
	{
		if (__DSP_first_task == nullptr || lbl_8049E101) {
			DSPSendMailToDSP(0xCDD10003);
			while (DSPCheckMailToDSP() != 0)
				;
			lbl_8049E101    = 0;
			__DSP_curr_task = DSP_prior_task;
			Dsp_Update_Request();
		} else {
			OSReport("Audio Yield Start\n");
			DSP_prior_yield = 3;
			DSPSendMailToDSP(0xCDD10001);
			while (DSPCheckMailToDSP() != 0)
				;
			__DSP_exec_task(DSP_prior_task, __DSP_first_task);
			__DSP_curr_task = __DSP_first_task;
			OSReport("Audio Yield Finish\n");
		}
		break;
	}
	}
	OSClearContext(&funcContext);
	OSSetCurrentContext(context);
}
END_SCOPE_EXTERN_C

/**
 * @TODO: Documentation
 */
void DsyncFrame2(u32 subframes, u32 dspbufStart, u32 dspbufEnd)
{
	if (DSP_prior_yield != TRUE) {
		OSReport("Yield中です\n");
		sync_stack[0] = subframes;
		sync_stack[1] = dspbufStart;
		sync_stack[2] = dspbufEnd;
		lbl_8049E101  = 1;
		return;
	}
	DsyncFrame2ch(subframes, dspbufStart, dspbufEnd);
	lbl_8049E101 = 0;
}

/**
 * @TODO: Documentation
 */
void DsyncFrame3(u32 param_0, u32 param_1, u32 param_2, u32 param_3, u32 param_4)
{
	if (DspRunningStatus != 1) {
		sync_stack[0] = param_0;
		sync_stack[1] = param_1;
		sync_stack[2] = param_2;
		sync_stack[3] = param_3;
		sync_stack[4] = param_4;
		lbl_8049E101  = 2;
		return;
	}
	DsyncFrame4ch(param_0, param_1, param_2, param_3, param_4);
	lbl_8049E101 = 0;
}

/**
 * @TODO: Documentation
 */
static void Dsp_Update_Request()
{
	switch (lbl_8049E101) {
	case 0:
		break;
	case 1:
		DsyncFrame2(sync_stack[0], sync_stack[1], sync_stack[2]);
		break;
	case 2:
		DsyncFrame3(sync_stack[0], sync_stack[1], sync_stack[2], sync_stack[3], sync_stack[4]);
		break;
	}
}

/**
 * @TODO: Documentation
 */
BOOL Dsp_Running_Check()
{
	return (DSP_prior_yield != TRUE) ? FALSE : TRUE;
}

/**
 * @TODO: Documentation
 */
void Dsp_Running_Start()
{
	DSP_prior_yield = 1;
}
