#ifndef NW4R_LYT_RESOURCES_H
#define NW4R_LYT_RESOURCES_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_types.h>

namespace nw4r {
namespace lyt {
namespace res {

/******************************************************************************
 *
 * BinaryFileHeader
 *
 ******************************************************************************/
struct BinaryFileHeader {
    char signature[4]; 	  // _00
    u16 byteOrder;        // _04
    u16 version;          // _06
    u32 fileSize;         // _08
    u16 headerSize;    	  // _0C
    u16 dataBlocks;       // _0E
};

/******************************************************************************
 *
 * DataBlockHeader
 *
 ******************************************************************************/
struct DataBlockHeader {
    char kind[4];    // _00
    u32 size;        // _04
};

/******************************************************************************
 *
 * Texture
 *
 ******************************************************************************/
struct Texture {
    u32 nameStrOffset;            // _00
    u8 type;                      // _04
    u8 PADDING_0x5[0x8 - 0x5];    // _05
};
/******************************************************************************
 *
 * TextureList
 *
 ******************************************************************************/
struct TextureList {
    res::DataBlockHeader blockHeader;  	 // _00
    u16 texNum;                        	 // _08
    u8 PADDING_0xA[0xC - 0xA];         	 // _0A
};

/******************************************************************************
 *
 * Font
 *
 ******************************************************************************/
struct Font {
    u32 nameStrOffset;        	     // _00
    u8 type;                   	     // _04
    u8 PADDING_0x5[0x8 - 0x5];  	 // _05
};
/******************************************************************************
 *
 * FontList
 *
 ******************************************************************************/
struct FontList {
    res::DataBlockHeader blockHeader;    // _00
    u16 fontNum;                         // _08
    u8 PADDING_0xA[0xC - 0xA];           // _0A
};

/******************************************************************************
 *
 * MaterialList
 *
 ******************************************************************************/
struct MaterialList {
    res::DataBlockHeader blockHeader;    // _00
    u16 materialNum;                     // _08
    u8 PADDING_0xA[0xC - 0xA];           // _0A
};

/******************************************************************************
 *
 * TexMap
 *
 ******************************************************************************/
struct TexMap {
	
	GXTexWrapMode GetWarpModeS() const {
        return static_cast<GXTexWrapMode>(wrapS & 3);
    }
    GXTexWrapMode GetWarpModeT() const {
        return static_cast<GXTexWrapMode>(wrapT & 3);
    }

    GXTexFilter GetMinFilter() const {
		u8 bitData = (wrapS >> 2) & 7;
        return static_cast<GXTexFilter>((bitData + 1) & 7);
    }
    GXTexFilter GetMagFilter() const {
		u8 bitData = (wrapT >> 2) & 1;
        return static_cast<GXTexFilter>((bitData + 1) & 1);
    }
	
    u16 texIdx;     // _00
    u8 wrapS;       // _02
    u8 wrapT;       // _03
};

/******************************************************************************
 *
 * MaterialResourceNum
 *
 ******************************************************************************/
struct MaterialResourceNum {
    u32 bits;

    u8 GetTexMapNum() const {
        return detail::GetBits(bits, 0, 4);
    }
    u8 GetTexSRTNum() const {
        return detail::GetBits(bits, 4, 4);
    }
    u8 GetTexCoordGenNum() const {
        return detail::GetBits(bits, 8, 4);
    }
    bool HasTevSwapTable() const {
        return detail::TestBit(bits, 12);
    }
    u8 GetIndTexSRTNum() const {
        return detail::GetBits(bits, 13, 2);
    }
    u8 GetIndTexStageNum() const {
        return detail::GetBits(bits, 15, 3);
    }
    u8 GetTevStageNum() const {
        return detail::GetBits(bits, 18, 5);
    }
    bool HasAlphaCompare() const {
        return detail::TestBit(bits, 23);
    }
    bool HasBlendMode() const {
        return detail::TestBit(bits, 24);
    }
    u8 GetChanCtrlNum() const {
        return detail::GetBits(bits, 25, 1);
    }
    u8 GetMatColNum() const {
        return detail::GetBits(bits, 27, 1);
    }
};


} // namespace res

/******************************************************************************
 *
 * ResBlockSet
 *
 ******************************************************************************/
struct ResBlockSet {
    const res::TextureList* pTextureList;      // _00
    const res::FontList* pFontList;            // _04
    const res::MaterialList* pMaterialList;    // _08
    ResourceAccessor* pResAccessor;            // _0C
};

} // namespace lyt
} // namespace nw4r

#endif
