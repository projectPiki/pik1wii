#ifndef _REVOSDK_OS_OSARENA_H
#define _REVOSDK_OS_OSARENA_H

#include "types.h"

BEGIN_SCOPE_EXTERN_C

// Arena functions.
void* OSGetArenaHi(void);
void* OSGetArenaLo(void);
void OSSetArenaHi(void* addr);
void OSSetArenaLo(void* addr);

void OSSetMEM1ArenaHi(void* addr);
void OSSetMEM1ArenaLo(void* addr);
void OSSetMEM2ArenaHi(void* addr);
void OSSetMEM2ArenaLo(void* addr);

void* OSGetMEM1ArenaLo(void);
void* OSGetMEM1ArenaHi(void);
void* OSGetMEM2ArenaLo(void);
void* OSGetMEM2ArenaHi(void);

END_SCOPE_EXTERN_C

#endif
