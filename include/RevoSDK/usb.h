#ifndef REVOSDK_USB_H
#define REVOSDK_USB_H

#include "RevoSDK/ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u8 bLength;
    u8 bDescriptorType;
    u16 bcdUSB;
    u8 bDeviceClass;
    u8 bDeviceSubClass;
    u8 bDeviceProtocol;
    u8 bMaxPacketSize0;
    u16 idVendor;
    u16 idProduct;
    u16 bcdDevice;
    u8 iManufacturer;
    u8 iProduct;
    u8 iSerialNumber;
    u8 bNumConfigurations;
} USB_DevDescr;

typedef struct IsoTransfer {
    void* buf;
    u8 numPackets;
    u16* packets;
} IsoTransfer;

typedef void (*USBCallback)(IPCResult result, void* arg);
typedef void (*USBIsoCallback)(IPCResult result, IsoTransfer* xfer, void* arg);

IPCResult IUSB_OpenLib(void);
IPCResult IUSB_CloseLib(void);
IPCResult IUSB_OpenDeviceIds(const char* interface, u16 vid, u16 pid,
                             IPCResult* resultOut);
IPCResult IUSB_CloseDeviceAsync(s32 fd, USBCallback callback,
                                void* callbackArg);
IPCResult IUSB_ReadIntrMsgAsync(s32 fd, u32 endpoint, u32 length, void* buffer,
                                USBCallback callback, void* callbackArg);
IPCResult IUSB_ReadBlkMsgAsync(s32 fd, u32 endpoint, u32 length, void* buffer,
                               USBCallback callback, void* callbackArg);
IPCResult IUSB_WriteBlkMsgAsync(s32 fd, u32 endpoint, u32 length,
                                void* buffer, USBCallback callback,
                                void* callbackArg);
IPCResult IUSB_WriteCtrlMsgAsync(s32 fd, u8 requestType, u8 request, u16 value,
                                 u16 index, u16 length, void* buffer,
                                 USBCallback callback, void* callbackArg);

#ifdef __cplusplus
}
#endif

#endif  // SC_H
