#ifndef NW4R_DB_DIRECT_PRINT_H
#define NW4R_DB_DIRECT_PRINT_H

#include <nw4r/types_nw4r.h>
#include <stdarg.h>
#include <RevoSDK/GX.h>

namespace nw4r {

namespace db {

struct FrameBufferInfo {
    u8 *frameMemory;      // _00
    u32 frameSize;        // _04
    u16 frameWidth;       // _08
    u16 frameHeight;      // _0A
    u16 frameRow;         // _0C
    u16 reserved;	      // _0E
};

struct YUVColorInfo {
    GXColor colorRGBA;    // _00
    u16 colorY256;        // _04
    u16 colorU;           // _06
    u16 colorU2;       	  // _08
    u16 colorU4;       	  // _0A
    u16 colorV;       	  // _0C
    u16 colorV2;    	  // _0E
    u16 colorV4;    	  // _10
    u16 reserved;         // _12
};

void DirectPrint_Init();
bool DirectPrint_IsActive();
void DirectPrint_EraseXfb(int posh, int posv, int sizeh, int sizev);
void DirectPrint_ChangeXfb(void *framBuf);
void DirectPrint_ChangeXfb(void *framBuf, u16 width, u16 height);
void DirectPrint_StoreCache();
void DirectPrint_Printf(int posh, int posv, const char *format, ...);
void DirectPrint_Printf(int posh, int posv, bool turnOver, const char *format, ...);
void DirectPrint_DrawString(int posh, int posv, const char *format, ...);
void DirectPrint_DrawString(int posh, int posv, bool turnOver, const char *format, ...);
void DirectPrint_SetColor(GXColor color);
void DirectPrint_SetColor(u8 r, u8 g, u8 b);
GXColor DirectPrint_GetColor();
namespace detail {
void DirectPrint_DrawStringToXfb(
    int posh, int posv, const char *format, va_list *args, bool turnover, bool backErase
);

void WaitVIRetrace_();

void *CreateFB_(const _GXRenderModeObj *rmode);
void *DirectPrint_SetupFB(const _GXRenderModeObj *rmode);

} // namespace detail

} // namespace db

} // namespace nw4r
#endif