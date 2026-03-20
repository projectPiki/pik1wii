#ifndef REVOSDK_WUD_HID_HOST_H
#define REVOSDK_WUD_HID_HOST_H

#include <RevoSDK/bte.h>
#include <types.h>


#ifdef __cplusplus
extern "C" {
#endif

void WUDHidHostCallback(tBTA_HH_EVT event, tBTA_HH* pData);

#ifdef __cplusplus
}
#endif
#endif