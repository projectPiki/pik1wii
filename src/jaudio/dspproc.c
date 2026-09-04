#include "jaudio/dspproc.h"
#include "RevoSDK/dsp.h"
#include "RevoSDK/os.h"
#include "jaudio/dspinterface.h"
#include <stddef.h>

static u16 DSP_MIXERLEVEL = 0x4000;
volatile static int flag;
volatile static int d_waitflag;

/**
 * @TODO: Documentation
 */
void DSPReleaseHalt2(u32 msg)
{
	u32 msgs[2];
	u16 dspMap = DSP_CreateMap2(msg);
	msgs[0]    = (msg << 16) | dspMap;

	DSPSendCommands2(msgs, 0, NULL);
}

u32 DSPReleaseHalt()
{
	u32 bit[4];
	bit[0] = 0x30000;
	DSPSendCommands2(bit, 0, NULL);
}

static void setup_callback(u16 a)
{
	OSReport("Finish %d\n", a);
	flag = FALSE;
}

/**
 * @TODO: Documentation
 */
s32 DSPSendCommands(u32* commands, u32 count)
{
	if (DSPCheckMailToDSP() != 0) {
		OSReport("DSP Err:not received mail (to DSP) is remained \n");
		return -1;
	}

	if (DSPCheckMailFromDSP() != 0) {
		OSReport("DSP Err:not received mail (from DSP) is remained \n");
		return -1;
	}

	int i;

	for (i = 0; i < count; i++) {
		DSPSendMailToDSP(commands[i]);

		while (DSPCheckMailToDSP() != 0)
			;
	}

	return 0;
}

/**
 * @TODO: Documentation
 */
void DSPWaitFinish()
{
	u32 mail;
	while (TRUE) {
		while (DSPCheckMailFromDSP() == 0)
			;

		mail = DSPReadMailFromDSP() + 0x77780000;
		if (mail != 0x1357) {
			return;
		}
		OSReport("Error: DSP now in framework\n");
	}
}

/**
 * @TODO: Documentation
 */
void DsetupTable(u32 cmd1, u32 cmd2, u32 cmd3, u32 cmd4, u32 cmd5)
{
	u32 commands[5];

	commands[0] = (cmd1 & 0xFFFF) | 0x81000000;
	commands[1] = cmd2;
	commands[2] = cmd3;
	commands[3] = cmd4;
	commands[4] = cmd5;
	OSReport("Table Setup\n");

	flag = 1;
	DSPSendCommands2(commands, ARRAY_SIZE(commands), setup_callback);
	while (flag != 0) { }
}

/**
 * @TODO: Documentation
 */
void DsetMixerLevel(f32 level)
{
	DSP_MIXERLEVEL = 4096.0f * level;
}

/**
 * @TODO: Documentation
 */
void DsyncFrame2ch(u32 subframes, u32 dspbufStart, u32 dspbufEnd)
{
	u32 commands[5];

	u32 val = (subframes << 16 & 0xFF0000);
	val |= 0x82000000;
	commands[0] = val | DSP_MIXERLEVEL;
	commands[1] = dspbufStart;
	commands[2] = dspbufEnd;
	commands[3] = 0;
	commands[4] = 0;

	DSPSendCommands2(commands, ARRAY_SIZE(commands), NULL);
}

/**
 * @TODO: Documentation
 */
void DsyncFrame4ch(u32 param_0, u32 param_1, u32 param_2, u32 param_3, u32 param_4)
{
	u32 commands[5];

	u32 val = (param_0 << 16 & 0xFF0000);
	val |= 0x82000000;
	commands[0] = val | DSP_MIXERLEVEL;
	commands[1] = param_1;
	commands[2] = param_2;
	commands[3] = param_3;
	commands[4] = param_4;
	DSPSendCommands2(commands, ARRAY_SIZE(commands), NULL);
}

/**
 * @TODO: Documentation
 */
static void dummy_callback(u16 param_0) {
    d_waitflag = FALSE;
    OSReport("D-Wait end\n", param_0);
}

/**
 * @TODO: Documentation
 */
void DsetVARAM(u32 param_0) {
    u32 msgs[2];
    msgs[0] = 0x8E000000;
    msgs[1] = param_0;

    d_waitflag = TRUE;
    DSPSendCommands2(msgs, 2, dummy_callback);
    do {
    } while (d_waitflag);
	
	lbl_8049E0F0 = param_0;
}
