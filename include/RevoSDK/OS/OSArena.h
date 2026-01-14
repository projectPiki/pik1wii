#ifndef _REVOSDK_OS_OSARENA_H
#define _REVOSDK_OS_OSARENA_H

#include "types.h"

BEGIN_SCOPE_EXTERN_C

// Arena functions.
void* OSGetArenaHi(void);
void* OSGetArenaLo(void);
void OSSetArenaHi(void* addr);
void OSSetArenaLo(void* addr);

END_SCOPE_EXTERN_C

#endif
