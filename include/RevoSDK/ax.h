#ifndef _REVOSDK_AX_H
#define _REVOSDK_AX_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AX_PRIORITY_FREE 0
#define AX_PRIORITY_MIN 1
#define AX_PRIORITY_MAX 31

#define AX_SAMPLES_PER_FRAME 96
#define AX_SAMPLE_DEPTH_BYTES sizeof(u32)
#define AX_SAMPLES_PER_FRAME_RMT 18
#define AX_FRAME_SIZE (AX_SAMPLES_PER_FRAME * AX_SAMPLE_DEPTH_BYTES)

#define AX_MAX_VOLUME 32768
// Command list can hold up to 64 commands
#define AX_CL_MAX_CMD 64
// Each command takes up two bytes
#define AX_CL_SIZE (AX_CL_MAX_CMD * sizeof(u16))

#define AX_SAMPLE_RATE 32000
#define AX_VOICE_MAX 96

#define AX_STREAM_SIZE_RMT 40
#define AX_RMT_MAX 4

/**
 * One frame contains eight bytes:
 * - One for the header
 * - Seven for the audio samples
 */
#define AX_ADPCM_FRAME_SIZE 8
#define AX_ADPCM_SAMPLE_BYTES_PER_FRAME (AX_ADPCM_FRAME_SIZE - 1)

// Two audio samples per byte (each nibble)
#define AX_ADPCM_SAMPLES_PER_BYTE 2

// Amount of audio samples in a frame
#define AX_ADPCM_SAMPLES_PER_FRAME                                             \
    (AX_ADPCM_SAMPLE_BYTES_PER_FRAME * AX_ADPCM_SAMPLES_PER_BYTE)

// Amount of nibbles in a frame
#define AX_ADPCM_NIBBLES_PER_FRAME (AX_ADPCM_FRAME_SIZE * 2)


/**
 * Stereo: Left, Right, Surround
 * DPL2:   Left, Right, Left Surround, Right Surround
 */
typedef enum {
    AX_STEREO_L,
    AX_STEREO_R,
    AX_STEREO_S,

    AX_STEREO_MAX
} AXStereoChannel;

typedef enum {
    AX_DPL2_L,
    AX_DPL2_R,
    AX_DPL2_LS,
    AX_DPL2_RS,

    AX_DPL2_MAX
} AXDPL2Channel;

typedef void (*AXAuxCallback)(void* chans, void* context);

typedef enum {
    AX_OUTPUT_STEREO,
    AX_OUTPUT_SURROUND,
    AX_OUTPUT_DPL2
} AXOutputMode;

typedef void (*AXOutCallback)(void);
typedef void (*AXExceedCallback)(u32 cycles);

typedef enum { AX_VOICE_NORMAL, AX_VOICE_STREAM } AXVOICETYPE;

typedef enum { AX_VOICE_STOP, AX_VOICE_RUN } AXVOICESTATE;

typedef enum {
    AX_SAMPLE_FORMAT_DSP_ADPCM = 0,
    AX_SAMPLE_FORMAT_PCM_S16 = 10,
    AX_SAMPLE_FORMAT_PCM_S8 = 25,
} AXSAMPLETYPE;

// For rmtIIR union I think? From NW4R asserts, but fits well in __AXSyncPBs
typedef enum {
    AX_PB_LPF_ON = 1,
    AX_PB_BIQUAD_ON,
};

typedef enum {
    AX_SRC_TYPE_NONE,
    AX_SRC_TYPE_LINEAR,
    AX_SRC_TYPE_4TAP_8K,
    AX_SRC_TYPE_4TAP_12K,
    AX_SRC_TYPE_4TAP_16K,
    AX_SRC_TYPE_4TAP_AUTO
} AXPBSRCTYPE;

typedef enum {
    AX_MIXER_CTRL_L = (1 << 0),
    AX_MIXER_CTRL_R = (1 << 1),
    AX_MIXER_CTRL_DELTA = (1 << 2),
    AX_MIXER_CTRL_S = (1 << 3),
    AX_MIXER_CTRL_DELTA_S = (1 << 4),

    AX_MIXER_CTRL_AL = (1 << 16),
    AX_MIXER_CTRL_AR = (1 << 17),
    AX_MIXER_CTRL_ADELTA = (1 << 18),
    AX_MIXER_CTRL_AS = (1 << 19),
    AX_MIXER_CTRL_ADELTA_S = (1 << 20),

    AX_MIXER_CTRL_BL = (1 << 21),
    AX_MIXER_CTRL_BR = (1 << 22),
    AX_MIXER_CTRL_BDELTA = (1 << 23),
    AX_MIXER_CTRL_BS = (1 << 24),
    AX_MIXER_CTRL_BDELTA_S = (1 << 25),

    AX_MIXER_CTRL_CL = (1 << 26),
    AX_MIXER_CTRL_CR = (1 << 27),
    AX_MIXER_CTRL_CDELTA = (1 << 28),
    AX_MIXER_CTRL_CS = (1 << 29),
    AX_MIXER_CTRL_CDELTA_S = (1 << 30)
};

typedef enum {
    AX_MIXER_CTRL_RMT_M0 = (1 << 0),
    AX_MIXER_CTRL_RMT_DELTA_M0 = (1 << 1),
    AX_MIXER_CTRL_RMT_A0 = (1 << 2),
    AX_MIXER_CTRL_RMT_DELTA_A0 = (1 << 3),

    AX_MIXER_CTRL_RMT_M1 = (1 << 4),
    AX_MIXER_CTRL_RMT_DELTA_M1 = (1 << 5),
    AX_MIXER_CTRL_RMT_A1 = (1 << 6),
    AX_MIXER_CTRL_RMT_DELTA_A1 = (1 << 7),

    AX_MIXER_CTRL_RMT_M2 = (1 << 8),
    AX_MIXER_CTRL_RMT_DELTA_M2 = (1 << 9),
    AX_MIXER_CTRL_RMT_A2 = (1 << 10),
    AX_MIXER_CTRL_RMT_DELTA_A2 = (1 << 11),

    AX_MIXER_CTRL_RMT_M3 = (1 << 12),
    AX_MIXER_CTRL_RMT_DELTA_M3 = (1 << 13),
    AX_MIXER_CTRL_RMT_A3 = (1 << 14),
    AX_MIXER_CTRL_RMT_DELTA_A3 = (1 << 15)
};

typedef struct _AXPBMIX {
    u16 vL;          // _00
    u16 vDeltaL;     // _02
    u16 vR;          // _04
    u16 vDeltaR;     // _06
    u16 vAuxAL;      // _08
    u16 vDeltaAuxAL; // _0A
    u16 vAuxAR;      // _0C
    u16 vDeltaAuxAR; // _0E
    u16 vAuxBL;      // _10
    u16 vDeltaAuxBL; // _12
    u16 vAuxBR;      // _14
    u16 vDeltaAuxBR; // _16
    u16 vAuxCL;      // _18
    u16 vDeltaAuxCL; // _1A
    u16 vAuxCR;      // _1C
    u16 vDeltaAuxCR; // _1E
    u16 vS;          // _20
    u16 vDeltaS;     // _22
    u16 vAuxAS;      // _24
    u16 vDeltaAuxAS; // _26
    u16 vAuxBS;      // _28
    u16 vDeltaAuxBS; // _2A
    u16 vAuxCS;      // _2C
    u16 vDeltaAuxCS; // _2E
} AXPBMIX;

typedef struct _AXPBITD {
    u16 flag;         // _00
    u16 bufferHi;     // _02
    u16 bufferLo;     // _04
    u16 shiftL;       // _06
    u16 shiftR;       // _08
    u16 targetShiftL; // _0A
    u16 targetShiftR; // _0C
} AXPBITD;

typedef struct _AXPBDPOP {
    s16 aL;     // _00
    s16 aAuxAL; // _02
    s16 aAuxBL; // _04
    s16 aAuxCL; // _06
    s16 aR;     // _08
    s16 aAuxAR; // _0A
    s16 aAuxBR; // _0C
    s16 aAuxCR; // _0E
    s16 aS;     // _10
    s16 aAuxAS; // _12
    s16 aAuxBS; // _14
    s16 aAuxCS; // _16
} AXPBDPOP;

typedef struct _AXPBVE {
    u16 currentVolume; // _00
    s16 currentDelta;  // _02
} AXPBVE;

typedef struct _AXPBADDR {
    u16 loopFlag;         // _00
    u16 format;           // _02
    u16 loopAddressHi;    // _04
    u16 loopAddressLo;    // _06
    u16 endAddressHi;     // _08
    u16 endAddressLo;     // _0A
    u16 currentAddressHi; // _0C
    u16 currentAddressLo; // _0E
} AXPBADDR;

typedef struct _AXPBADPCM {
    u16 a[8][2];    // _00
    u16 gain;       // _20
    u16 pred_scale; // _22
    u16 yn1;        // _24
    u16 yn2;        // _26
} AXPBADPCM;

typedef struct _AXPBSRC {
    u16 ratioHi;            // _00
    u16 ratioLo;            // _02
    u16 currentAddressFrac; // _04
    u16 last_samples[4];    // _06
} AXPBSRC;

typedef struct _AXPBADPCMLOOP {
    u16 loop_pred_scale; // _00
    u16 loop_yn1;        // _02
    u16 loop_yn2;        // _04
} AXPBADPCMLOOP;

typedef struct _AXPBLPF {
    u16 on;  // _00
    u16 yn1; // _02
    u16 a0;  // _04
    u16 b0;  // _06
} AXPBLPF;

typedef struct _AXPBBIQUAD {
    u16 on;  // _00
    u16 xn1; // _02
    u16 xn2; // _04
    u16 yn1; // _06
    u16 yn2; // _08
    u16 b0;  // _0A
    u16 b1;  // _0C
    u16 b2;  // _0E
    u16 a1;  // _10
    u16 a2;  // _12
} AXPBBIQUAD;

typedef struct _AXPBRMTMIX {
    u16 vMain0;      // _00
    u16 vDeltaMain0; // _02
    u16 vAux0;       // _04
    u16 vDeltaAux0;  // _06
    u16 vMain1;      // _08
    u16 vDeltaMain1; // _0A
    u16 vAux1;       // _0C
    u16 vDeltaAux1;  // _0E
    u16 vMain2;      // _10
    u16 vDeltaMain2; // _12
    u16 vAux2;       // _14
    u16 vDeltaAux2;  // _16
    u16 vMain3;      // _18
    u16 vDeltaMain3; // _1A
    u16 vAux3;       // _1C
    u16 vDeltaAux3;  // _1E
} AXPBRMTMIX;

typedef struct _AXPBRMTDPOP {
    s16 aMain0; // _00
    s16 aMain1; // _02
    s16 aMain2; // _04
    s16 aMain3; // _06
    s16 aAux0;  // _08
    s16 aAux1;  // _0A
    s16 aAux2;  // _0C
    s16 aAux3;  // _0E
} AXPBRMTDPOP;

typedef struct _AXPBRMTSRC {
    u16 currentAddressFrac; // _00
    u16 last_samples[4];    // _02
} AXPBRMTSRC;

typedef union __AXPBRMTIIR {
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
} AXPBRMTIIR;

typedef struct _AXPB {
    u16 nextHi;                // _00
    u16 nextLo;                // _02
    u16 currHi;                // _04
    u16 currLo;                // _06
    u16 srcSelect;             // _08
    u16 coefSelect;            // _0A
    u32 mixerCtrl;             // _0C
    u16 state;                 // _10
    u16 type;                  // _12
    AXPBMIX mix;               // _14
    AXPBITD itd;               // _44
    AXPBDPOP dpop;             // _52
    AXPBVE ve;                 // _6A
    AXPBADDR addr;             // _6E
    AXPBADPCM adpcm;           // _7E
    AXPBSRC src;               // _A6
    AXPBADPCMLOOP adpcmLoop;   // _B4
    AXPBLPF lpf;               // _BA
    AXPBBIQUAD biquad;         // _C2
    u16 remote;                // _D6
    u16 rmtMixerCtrl;          // _D8
    AXPBRMTMIX rmtMix;         // _DA
    AXPBRMTDPOP rmtDpop;       // _FA
    AXPBRMTSRC rmtSrc;         // _10A
    AXPBRMTIIR rmtIIR;         // _114
    u8 padding[0x140 - 0x128]; // _128
} AXPB;

typedef struct _AXPROFILE {
    s64 timeBegin;           // _00
    s64 timeProcessAuxBegin; // _08
    s64 timeProcessAuxEnd;   // _10
    s64 timeUserFrameBegin;  // _18
    s64 timeUserFrameEnd;    // _20
    s64 timeEnd;             // _28
    s32 numVoices;           // _30
    s32 WORD_0x34;
} AXPROFILE;

#pragma pack(push, 1)
typedef struct _AXSTUDIO {
    s32 L;
    s16 dL;

    s32 R;
    s16 dR;

    s32 S;
    s16 dS;

    s32 AuxAL;
    s16 dAuxAL;

    s32 AuxAR;
    s16 dAuxAR;

    s32 AuxAS;
    s16 dAuxAS;

    s32 AuxBL;
    s16 dAuxBL;

    s32 AuxBR;
    s16 dAuxBR;

    s32 AuxBS;
    s16 dAuxBS;

    s32 AuxCL;
    s16 dAuxCL;

    s32 AuxCR;
    s16 dAuxCR;

    s32 AuxCS;
    s16 dAuxCS;

    s32 Main0;
    s16 dMain0;

    s32 Aux0;
    s16 dAux0;

    s32 Main1;
    s16 dMain1;

    s32 Aux1;
    s16 dAux1;

    s32 Main2;
    s16 dMain2;

    s32 Aux2;
    s16 dAux2;

    s32 Main3;
    s16 dMain3;

    s32 Aux3;
    s16 dAux3;
} AXSTUDIO;
#pragma pack(pop)

typedef enum {
    AX_PBSYNC_SELECT = (1 << 0),
    AX_PBSYNC_MIXER_CTRL = (1 << 1),
    AX_PBSYNC_STATE = (1 << 2),
    AX_PBSYNC_TYPE = (1 << 3),
    AX_PBSYNC_MIX = (1 << 4),
    AX_PBSYNC_ITD = (1 << 5),
    AX_PBSYNC_ITD_SHIFT = (1 << 6),
    AX_PBSYNC_DPOP = (1 << 7),
    AX_PBSYNC_VE = (1 << 8),
    AX_PBSYNC_VE_DELTA = (1 << 9),
    AX_PBSYNC_ADDR = (1 << 10),
    AX_PBSYNC_LOOP_FLAG = (1 << 11),
    AX_PBSYNC_LOOP_ADDR = (1 << 12),
    AX_PBSYNC_END_ADDR = (1 << 13),
    AX_PBSYNC_CURR_ADDR = (1 << 14),
    AX_PBSYNC_ADPCM = (1 << 15),
    AX_PBSYNC_SRC = (1 << 16),
    AX_PBSYNC_SRC_RATIO = (1 << 17),
    AX_PBSYNC_ADPCM_LOOP = (1 << 18),
    AX_PBSYNC_LPF = (1 << 19),
    AX_PBSYNC_LPF_COEFS = (1 << 20),
    AX_PBSYNC_BIQUAD = (1 << 21),
    AX_PBSYNC_BIQUAD_COEFS = (1 << 22),
    AX_PBSYNC_REMOTE = (1 << 23),
    AX_PBSYNC_RMT_MIXER_CTRL = (1 << 24),
    AX_PBSYNC_RMTMIX = (1 << 25),
    AX_PBSYNC_RMTDPOP = (1 << 26),
    AX_PBSYNC_RMTSRC = (1 << 27),
    AX_PBSYNC_RMTIIR = (1 << 28),
    AX_PBSYNC_RMTIIR_LPF_COEFS = (1 << 29),
    AX_PBSYNC_RMTIIR_BIQUAD_COEFS = (1 << 30),
    AX_PBSYNC_ALL = (1 << 31),
};

//typedef void (*AXCallback)();
typedef void (*AXVoiceCallback)(void* vpb);

typedef struct _AXVPB {
    void* next;               // _0
    void* prev;               // _4
    void* next1;              // _8
    u32 priority;             // _C
    AXVoiceCallback callback; // _10
    u32 userContext;          // _14
    u32 index;                // _18
    u32 sync;                 // _1C
    u32 depop;                // _20
    void* itdBuffer;          // _24
    AXPB pb;                  // _28
} AXVPB;

extern u32 __AXClMode;

extern u16 __AXCompressorTable[];

extern u16 axDspInitVector;
extern u16 axDspResumeVector;

extern u16 axDspSlave[];
extern u16 axDspSlaveLength;


void AXInit(void);
BOOL AXIsInit(void);
void AXInitEx(u32 mode);

void __AXAuxInit(void);
void __AXAuxQuit(void);
void __AXGetAuxAInput(void** out);
void __AXGetAuxAOutput(void** out);
void __AXGetAuxAInputDpl2(void** out);
void __AXGetAuxAOutputDpl2R(void** out);
void __AXGetAuxAOutputDpl2Ls(void** out);
void __AXGetAuxAOutputDpl2Rs(void** out);
void __AXGetAuxBInput(void** out);
void __AXGetAuxBOutput(void** out);
void __AXGetAuxBInputDpl2(void** out);
void __AXGetAuxBOutputDpl2R(void** out);
void __AXGetAuxBOutputDpl2Ls(void** out);
void __AXGetAuxBOutputDpl2Rs(void** out);
void __AXGetAuxCInput(void** out);
void __AXGetAuxCOutput(void** out);
void __AXProcessAux(void);
void AXRegisterAuxACallback(AXAuxCallback callback, void* context);
void AXRegisterAuxBCallback(AXAuxCallback callback, void* context);
void AXRegisterAuxCCallback(AXAuxCallback callback, void* context);
void AXGetAuxACallback(AXAuxCallback* callback, void** context);
void AXGetAuxBCallback(AXAuxCallback* callback, void** context);
void AXGetAuxCCallback(AXAuxCallback* callback, void** context);

u32 __AXGetCommandListCycles(void);
u32 __AXGetCommandListAddress(void);
void __AXWriteToCommandList(u16 cmd);
void __AXNextFrame(void* surround, void* lr, void* rmt);
void __AXClInit(void);
void __AXClQuit(void);
void AXSetMode(u32 mode);
u32 AXGetMode(void);
u16 AXGetAuxAReturnVolume(void);
u16 AXGetAuxBReturnVolume(void);
u16 AXGetAuxCReturnVolume(void);
void AXSetMasterVolume(u16 volume);
void AXSetAuxAReturnVolume(u16 volume);
void AXSetAuxBReturnVolume(u16 volume);
void AXSetAuxCReturnVolume(u16 volume);

u32 __AXOutNewFrame(void);
void __AXOutAiCallback(void);
void __AXOutInitDSP(void);
void __AXOutInit(u32 mode);
void __AXOutQuit(void);
AXOutCallback AXRegisterCallback(AXOutCallback callback);
s32 AXRmtGetSamplesLeft(void);
s32 AXRmtGetSamples(s32 chan, s16* out, s32 num);
s32 AXRmtAdvancePtr(s32 num);

AXPROFILE* __AXGetCurrentProfile(void);

AXSTUDIO* __AXGetStudio(void);
void __AXDepopFadeMain(s32* all, s32* value, s16* depop);
void __AXDepopFadeRmt(s32* all, s32* value, s16* depop);
void __AXPrintStudio(void);
void __AXSPBInit(void);
void __AXSPBQuit(void);
void __AXDepopVoice(AXPB* pb);

s32 __AXGetNumVoices(void);
void __AXServiceVPB(AXVPB* vpb);
void __AXDumpVPB(AXVPB* vpb);
void __AXSyncPBs(u32 baseCycles);
AXPB* __AXGetPBs(void);
void __AXSetPBDefault(AXVPB* vpb);
void __AXVPBInit(void);
void __AXVPBQuit(void);
void AXSetVoiceState(AXVPB* vpb, u16 state);
void AXSetVoiceAddr(AXVPB* vpb, AXPBADDR* addr);
void AXGetLpfCoefs(u16 freq, u16* a, u16* b);
void AXSetMaxDspCycles(u32 num);
s32 AXGetMaxVoices(void);

AXVPB* __AXGetStackHead(u32 prio);
void __AXServiceCallbackStack(void);
void __AXInitVoiceStacks(void);
void __AXAllocInit(void);
void __AXAllocQuit(void);
void __AXPushFreeStack(AXVPB* vpb);
AXVPB* __AXPopFreeStack(void);
void __AXPushCallbackStack(AXVPB* vpb);
AXVPB* __AXPopCallbackStack(void);
void __AXRemoveFromStack(AXVPB* vpb);
void __AXPushStackHead(AXVPB* vpb, u32 prio);
AXVPB* __AXPopStackFromBottom(u32 prio);
void AXFreeVoice(AXVPB* vpb);
AXVPB* AXAcquireVoice(u32 prio, AXVoiceCallback callback, u32 userContext);
void AXSetVoicePriority(AXVPB* vpb, u32 prio);

#ifdef __cplusplus
}
#endif
#endif