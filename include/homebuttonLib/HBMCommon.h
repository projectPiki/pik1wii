#ifndef REVOSDK_HBM_COMMON_H
#define REVOSDK_HBM_COMMON_H

#include <types.h>

extern "C" MEMAllocator *spAllocator;

extern "C++" void *HBMAllocMem(u32 size);
extern "C++" void HBMFreeMem(void *mem);

#endif // REVOSDK_HBM_COMMON_H
