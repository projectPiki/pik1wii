#ifndef REVOSDK_BTE_DATA_TYPES_H
#define REVOSDK_BTE_DATA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define BCM_STRNCPY_S(x1, x2, x3, x4)	strncpy(x1, x3, x4)

/* bte uses different types so we will respect them here */

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef signed long INT32;
typedef signed char INT8;
typedef signed short INT16;
typedef unsigned char BOOLEAN;

typedef unsigned char UBYTE;

#ifdef __cplusplus
}
#endif

#endif  // REVOSDK_BTE_DATA_TYPES_H
