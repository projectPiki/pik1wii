#ifndef REVOSDK_SC_H
#define REVOSDK_SC_H

#include "RevoSDK/bte.h"
#include <RevoSDK/nand.h>
#include <RevoSDK/os.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_MAX_DEV_ENTRY_FOR_STD 10
#define SC_MAX_DEV_ENTRY_FOR_SMP 6
#define SC_MAX_DEV_ENTRY (SC_MAX_DEV_ENTRY_FOR_STD + SC_MAX_DEV_ENTRY_FOR_SMP)

typedef u8 SCType;

typedef enum {
    SC_STATUS_OK,
    SC_STATUS_BUSY,
    SC_STATUS_FATAL,
    SC_STATUS_PARSE
} SCStatus;

typedef struct {
    u8 mode;
    u8 led;
} SCIdleModeInfo;

typedef struct {
    union {
        u8 type_u8;
        s8 type_s8;
        u16 type_u16;
        s16 type_s16;
        u32 type_u32;
        s32 type_s32;
        u64 type_u64;
        s64 type_s64;
        u8 longPrecision64[sizeof(u64)];
    } integer;

    SCType typeInteger;
    SCType typeByteArray;
    u32 nameLen;
    u32 dataSize;
    char* name;
    u8* data;
    u32 packedSize;
} SCItem;

typedef struct {
    BD_ADDR bd_addr;
    u8 bd_name[64];
    u8 link_key[16];
} SCBtCmpDevInfoSingle;

typedef struct {
    BD_ADDR bd_addr;
    u8 bd_name[64];
} SCBtDeviceInfoSingle;

typedef struct {
    u8 num;
    SCBtCmpDevInfoSingle info[6];
} SCBtCmpDevInfoArray;

typedef struct SCDevInfo {
    char devName[20];  // _00
    char at_0x14[1];
    char UNK_0x15[0xB];
    LINK_KEY linkKey;  // _20
    char UNK_0x30[0x10];
} SCDevInfo;

typedef struct SCBtDeviceInfo {
    BD_ADDR addr;    // _00
    SCDevInfo info;  // _06
} SCBtDeviceInfo;

typedef struct SCBtDeviceInfoArray {
    u8 numRegist; // at 0x0
    union {
        struct {
            SCBtDeviceInfo regist[SC_MAX_DEV_ENTRY_FOR_STD]; // at 0x1
            SCBtDeviceInfo active[SC_MAX_DEV_ENTRY_FOR_SMP]; // at 0x2BD
        };

        SCBtDeviceInfo devices[SC_MAX_DEV_ENTRY];
    };
} SCBtDeviceInfoArray;

typedef void (*SCReloadConfFileCallback)(s32 result);
typedef void (*SCFlushCallback)(SCStatus status);


typedef enum {
    SC_ITEM_ID_IPL_COUNTER_BIAS,
    SC_ITEM_ID_IPL_ASPECT_RATIO,
    SC_ITEM_ID_IPL_AUTORUN_MODE,
    SC_ITEM_ID_IPL_CONFIG_DONE,
    SC_ITEM_ID_IPL_CONFIG_DONE2,
    SC_ITEM_ID_IPL_DISPLAY_OFFSET_H,
    SC_ITEM_ID_IPL_EURGB60_MODE,
    SC_ITEM_ID_IPL_EULA,
    SC_ITEM_ID_IPL_FREE_CHANNEL_APP_COUNT,
    SC_ITEM_ID_IPL_IDLE_MODE,
    SC_ITEM_ID_IPL_INSTALLED_CHANNEL_APP_COUNT,
    SC_ITEM_ID_IPL_LANGUAGE,
    SC_ITEM_ID_IPL_OWNER_NICKNAME,
    SC_ITEM_ID_IPL_PARENTAL_CONTROL,
    SC_ITEM_ID_IPL_PROGRESSIVE_MODE,
    SC_ITEM_ID_IPL_SCREEN_SAVER_MODE,
    SC_ITEM_ID_IPL_SIMPLE_ADDRESS,
    SC_ITEM_ID_IPL_SOUND_MODE,
    SC_ITEM_ID_IPL_UPDATE_TYPE,
    SC_ITEM_ID_NET_CONFIG,
    SC_ITEM_ID_NET_CONTENT_RESTRICTIONS,
    SC_ITEM_ID_NET_PROFILE,
    SC_ITEM_ID_NET_WC_RESTRICTION,
    SC_ITEM_ID_NET_WC_FLAGS,
    SC_ITEM_ID_DEV_BOOT_MODE,
    SC_ITEM_ID_DEV_VIDEO_MODE,
    SC_ITEM_ID_DEV_COUNTRY_CODE,
    SC_ITEM_ID_DEV_DRIVESAVING_MODE,
    SC_ITEM_ID_BT_DEVICE_INFO,
    SC_ITEM_ID_BT_CMPDEV_INFO,
    SC_ITEM_ID_BT_DPD_SENSIBILITY,
    SC_ITEM_ID_BT_SPEAKER_VOLUME,
    SC_ITEM_ID_BT_MOTOR_MODE,
    SC_ITEM_ID_BT_SENSOR_BAR_POSITION,
    SC_ITEM_ID_DVD_CONFIG,
    SC_ITEM_ID_WWW_RESTRICTION,
    SC_ITEM_ID_MAX_PLUS1
} SCItemID;

typedef enum {
    SC_AREA_JPN,
    SC_AREA_USA,
    SC_AREA_EUR,
    SC_AREA_AUS,
    SC_AREA_BRA,
    SC_AREA_TWN,
    SC_AREA_KOR,
    SC_AREA_HKG,
    SC_AREA_ASI,
    SC_AREA_LTN,
    SC_AREA_SAF,
} SCProductArea;

typedef struct SCConfHeader {
    u32 magic;         // at 0x0
    u16 numItems;      // at 0x4
    u16 itemOffsets[]; // at 0x6
} SCConfHeader;

typedef struct SCConfItem {
    // & 00011111 -> name length (-1)
    // & 11100000 -> item type
    u8 desc;     // at 0x0
    char name[]; // at 0x4
} SCConfItem;

typedef struct SCControl {
    OSThreadQueue threadQueue;
    NANDFileInfo nandFileInfo;
    NANDCommandBlock nandCommandBlock;

    union {
        u8 nandType;
        NANDStatus nandStatus;
    };

    u8 nandCbState;
    u8 nandNeedClose;
    u8 reloadFileCount;
    SCReloadConfFileCallback reloadCallback;
    s32 reloadResult;
    const char* reloadFileName[2];
    u8* reloadBufp[2];
    u32 reloadSizeExpected[2];
    u32 reloadedSize[2];
    SCFlushCallback flushCallback;
    u32 flushResult;
    u32 flushSize;
} SCControl;


typedef enum { SC_ASPECT_STD, SC_ASPECT_WIDE } SCAspectRatio;

typedef enum { SC_EURGB_50_HZ, SC_EURGB_60_HZ } SCEuRgb60Mode;

typedef enum {
    SC_LANG_JP,
    SC_LANG_EN,
    SC_LANG_DE,
    SC_LANG_FR,
    SC_LANG_SP,
    SC_LANG_IT,
    SC_LANG_NL,
    SC_LANG_ZH_S,
    SC_LANG_ZH_T,
    SC_LANG_KR,
} SCLanguage;

typedef enum { SC_INTERLACED, SC_PROGRESSIVE } SCProgressiveMode;

typedef enum { SC_MOTOR_OFF, SC_MOTOR_ON } SCMotorMode;

typedef enum { SC_SND_MONO, SC_SND_STEREO, SC_SND_SURROUND } SCSoundMode;

typedef enum { SC_SENSOR_BAR_BOTTOM, SC_SENSOR_BAR_TOP } SCSensorBarPos;

typedef enum {
    NAND_CB_STATE_0,
    NAND_CB_STATE_1,
    NAND_CB_STATE_2,
    NAND_CB_STATE_3,
    NAND_CB_STATE_4,
    NAND_CB_STATE_5,
    NAND_CB_STATE_6,
    NAND_CB_STATE_7,
    NAND_CB_STATE_8,
    NAND_CB_STATE_9
} NandCallbackState;

typedef enum {
    SC_CONF_FILE_SYSTEM,  // SYSCONF
    SC_CONF_FILE_PRODUCT, // setting.txt
    SC_CONF_FILE_MAX
} SCConfFile;

typedef enum {
    SC_ITEM_BIGARRAY = (1 << 5),
    SC_ITEM_SMALLARRAY = (2 << 5),
    SC_ITEM_BYTE = (3 << 5),
    SC_ITEM_SHORT = (4 << 5),
    SC_ITEM_LONG = (5 << 5),
    SC_ITEM_LONGLONG = (6 << 5),
    SC_ITEM_BOOL = (7 << 5)
} SCItemType;


s32 SCReloadConfFileAsync(u8*, u32, SCReloadConfFileCallback);


u8 SCGetLanguage(void);

u8* __SCGetConfBuf(void);
u32 __SCGetConfBufSize(void);

void SCInit(void);
u32 SCCheckStatus(void);
u32 SCGetCounterBias(void);
u8 SCGetSoundMode(void);
u8 SCGetAspectRatio(void);
u8 SCGetProgressiveMode(void);
u8 SCGetEuRgb60Mode(void);

BOOL SCGetIdleMode(SCIdleModeInfo*);

BOOL SCFindByteArrayItem(void*, u32, SCItemID);
BOOL SCFindU8Item(u8*, SCItemID);
BOOL SCFindS8Item(s8*, SCItemID);
BOOL SCFindU32Item(u32*, SCItemID);

s8 SCGetProductGameRegion(void);

BOOL __SCF1(const char*, char*, u32);

s8 SCGetDisplayOffsetH(void);
s8 SCGetProductArea(void);
u8 SCGetScreenSaverMode(void);

u32 SCGetBtDpdSensibility(void);
u8 SCGetWpadSensorBarPosition(void);
u8 SCGetWpadMotorMode(void);
u8 SCGetWpadSpeakerVolume(void);

void SCGetBtDeviceInfoArray(SCBtDeviceInfoArray* info);
BOOL SCSetBtDeviceInfoArray(const SCBtDeviceInfoArray*);
void SCGetBtCmpDevInfoArray(SCBtCmpDevInfoArray* info);
BOOL SCSetBtCmpDevInfoArray(const SCBtCmpDevInfoArray*);
void SCFlushAsync(SCFlushCallback);

BOOL SCSetWpadSpeakerVolume(u8 volume);
BOOL SCSetWpadMotorMode(u8 mode);

#ifdef __cplusplus
}
#endif

#endif  // SC_H
