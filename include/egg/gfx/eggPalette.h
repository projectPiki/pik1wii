#ifndef EGG_GFX_PALETTE_H
#define EGG_GFX_PALETTE_H

#include <egg/egg_types.h>
#include <RevoSDK/gx.h>

namespace EGG {

enum JUTTransparency {
    JUT_TRANSPARENCY_DISABLE,
    JUT_TRANSPARENCY_ENABLE,
};
struct ResTLUT {
    u8 format;
    u8 transparency;
    u16 numColors;
};

class Palette {
public:
    Palette(GXTlut p1, GXTlutFmt p2, JUTTransparency p3, u16 p4, void *p5) {
        storeTLUT(p1, p2, p3, p4, p5);
    }

    Palette(GXTlut tlutNo, ResTLUT *p_tlutRes) {
        storeTLUT(tlutNo, p_tlutRes);
    }

    void storeTLUT(GXTlut, ResTLUT *);
    void storeTLUT(GXTlut, GXTlutFmt, JUTTransparency, u16, void *);
    bool load();

    u8 getTlutName() const { return mTlutName; }
	
	u8 getFormat() const { return mFormat; }
	
	u8 getTransparency() const { return mTransparency; }
	
	u16 getNumColors() const { return mNumColors; }
	
	ResTLUT *getColorTable() const { return mColorTable; }
	
private:
    GXTlutObj mTlutObj; 	// _00
    u8 mTlutName; 			// _0C
    u8 mFormat; 			// _0D
    ResTLUT *mColorTable; 	// _10
    u16 mNumColors; 		// _14
    u8 mTransparency; 		// _16
};

} // namespace EGG

#endif