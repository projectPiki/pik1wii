#ifndef _REVOSDK_AXFX_H
#define _REVOSDK_AXFX_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

typedef struct AXFX_BUS {
    s32* left;   		  // _00
    s32* right;   		  // _04
    s32* surround;   	  // _08

} AXFX_BUS;

typedef struct AXFX_BUFFERUPDATE {
    s32* left;   		  // _00
    s32* right;   		  // _04
    s32* surround;   	  // _08

} AXFX_BUFFERUPDATE;

typedef struct AXFX_REVERBHI_EXP {
    f32* earlyLine[3];   		  // _00
    u32 earlyPos[3];     		  // _0C
    u32 earlyLength;     		  // _18
    u32 earlyMaxLength;  		  // _1C
    f32 earlyCoef[3];    		  // _20
    f32* preDelayLine[3];   	  // _2C
    u32 preDelayPos;        	  // _38
    u32 preDelayLength;     	  // _3C
    u32 preDelayMaxLength;  	  // _40
    f32* combLine[3][3];   		  // _44
    u32 combPos[3];        		  // _68
    u32 combLength[3];     		  // _74
    u32 combMaxLength[3];  		  // _80
    f32 combCoef[3];       		  // _8C
    f32* allpassLine[3][2];   	  // _98
    u32 allpassPos[2];        	  // _B0
    u32 allpassLength[2];     	  // _B8
    u32 allpassMaxLength[2];  	  // _C0
    f32* lastAllpassLine[3];      // _C8
    u32 lastAllpassPos[3];        // _D4
    u32 lastAllpassLength[3];     // _E0
    u32 lastAllpassMaxLength[3];  // _EC
    f32 allpassCoef;      		  // _F8
    f32 lastLpfOut[3];    		  // _FC
    f32 lpfCoef;          		  // _108
    u32 active;           		  // _10C
    u32 earlyMode;        		  // _110
    f32 preDelayTimeMax;  		  // _114
    f32 preDelayTime;     		  // _118
    u32 fusedMode;        		  // _11C
    f32 fusedTime;        		  // _120
    f32 coloration;       		  // _124
    f32 damping;          		  // _128
    f32 crosstalk;        		  // _12C
    f32 earlyGain;        		  // _130
    f32 fusedGain;        		  // _134
    AXFX_BUS* busIn;      		  // _138
    AXFX_BUS* busOut;     		  // _13C
    f32 outGain;          		  // _140
    f32 sendGain;         		  // _144
} AXFX_REVERBHI_EXP;

typedef struct AXFX_REVERBHI {
    AXFX_REVERBHI_EXP exp;  // _00
    f32 coloration;         // _148
    f32 mix;                // _14C
    f32 time;               // _150
    f32 damping;            // _154
    f32 preDelay;           // _158
    f32 crosstalk;          // _15C
} AXFX_REVERBHI;

typedef void* (*AXFXAllocHook)(size_t size);
typedef void (*AXFXFreeHook)(void* block);

extern AXFXAllocHook __AXFXAlloc;
extern AXFXFreeHook __AXFXFree;

void AXFXSetHooks(AXFXAllocHook alloc, AXFXFreeHook free);
void AXFXGetHooks(AXFXAllocHook* alloc, AXFXFreeHook* free);

BOOL AXFXReverbHiInit(AXFX_REVERBHI *reverbHi);
BOOL AXFXReverbHiShutdown(AXFX_REVERBHI *reverbHi);
void AXFXReverbHiCallback(void *data, void *context);

BOOL AXFXReverbHiExpInit(AXFX_REVERBHI_EXP* fx);
void AXFXReverbHiExpShutdown(AXFX_REVERBHI_EXP* fx);
void AXFXReverbHiExpCallback(AXFX_BUFFERUPDATE* update, AXFX_REVERBHI_EXP* fx);

#ifdef __cplusplus
}
#endif

#endif
