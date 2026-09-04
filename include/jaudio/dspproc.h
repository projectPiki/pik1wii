#ifndef _JAUDIO_DSPPROC_H
#define _JAUDIO_DSPPROC_H

#include "types.h"

/////////// JAUDIO DSP PROC DEFINITIONS ///////////
// Global functions (all C++, so no extern C wrap).
s32 DSPSendCommands(u32* commands, u32 count);
void DSPReleaseHalt2(u32 msg);
u32 DSPReleaseHalt();
void DSPWaitFinish();
void DsetupTable(u32 cmd1, u32 cmd2, u32 cmd3, u32 cmd4, u32 cmd5);
void DsetMixerLevel(f32 level);
void DsyncFrame2ch(u32 subframes, u32 dspbufStart, u32 dspbufEnd);
void DsyncFrame4ch(u32 param_0, u32 param_1, u32 param_2, u32 param_3, u32 param_4);
void DwaitFrame();
void DwaitFrame();
void DsetVARAM(u32 param_0);

#if defined(VERSION_GPIP01_00)
void DsyncFrame2(u32 subframes, u32 dspbufStart, u32 dspbufEnd);
#endif

u32 lbl_8049E0F0;

///////////////////////////////////////////////////

#endif
