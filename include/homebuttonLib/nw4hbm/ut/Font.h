#ifndef NW4HBM_UT_FONT_H
#define NW4HBM_UT_FONT_H

#include "homebuttonLib/nw4hbm/ut/CharStrmReader.h"
#include <RevoSDK/gx/GXEnum.h>
#include <types.h>


namespace nw4hbm { 
namespace ut {

	enum FontEncoding
	{
		FONT_ENCODING_UTF8,
		FONT_ENCODING_UTF16,
		FONT_ENCODING_SJIS,
		FONT_ENCODING_CP1252,

		NUM_OF_FONT_ENCODING
	};

	typedef u16 FontMapMethod;
	enum FontMapMethod_et
	{
		FONT_MAPMETHOD_DIRECT,
		FONT_MAPMETHOD_TABLE,
		FONT_MAPMETHOD_SCAN	
	};

	
	struct CharWidths {
		s8 left;			// _00
		u8 glyphWidth;		// _01
		s8 charWidth;		// _02
	}; 

	struct FontCodeMap {
		u16 ccodeBegin;					// _00
		u16 ccodeEnd;					// _02
		FontMapMethod mappingMethod;	// _04
		FontCodeMap *pNext;				// _08
		u16 mapInfo[];					// _0C
	}; 

	struct FontTextureGlyph {
		u8 cellWidth;			// _00
		u8 cellHeight;			// _01
		s8 baselinePos;			// _02
		u8 maxCharWidth;		// _03
		u32 sheetSize;			// _04
		u16 sheetNum;			// _08
		u16 sheetFormat;		// _0A
		u16 sheetRow;			// _0C
		u16 sheetLine;			// _0E
		u16 sheetWidth;			// _10
		u16 sheetHeight;		// _12
		u8	*sheetImage;	// _14
	};

	struct FontWidth {
		u16 indexBegin;				// _00
		u16 indexEnd;				// _02
		FontWidth *pNext;			// _04
		CharWidths widthTable[];	// _08
	};

	struct FontInformation {
		u8 fontType;					// _00
		s8 linefeed;					// _01
		u16 alterCharIndex;				// _02
		CharWidths defaultWidth;		// _04
		u8 encoding;					// _07
		FontTextureGlyph *pGlyph;		// _08
		FontWidth *pWidth;				// _0C
		FontCodeMap *pMap;				// _10
		u8 height;						// _14
		u8 width;						// _15
		u8 ascent;						// _16
	};

	struct Glyph {
		void *pTexture;			// _00
		CharWidths widths;		// _04
		u8 height;				// _07
		GXTexFmt texFormat;		// _08
		u16 texWidth;			// _0C
		u16 texHeight;			// _0E
		u16 cellX;				// _10
		u16 cellY;				// _12
	};

class Font {
public:
	enum Type
	{
		TYPE_NULL,

		TYPE_ROM,
		TYPE_RESOURCE
	};

	Font()
		:mReaderFunc(&CharStrmReader::ReadNextCharCP1252)
	{
	}

	virtual ~Font() {}													// _08
    virtual int GetWidth() const = 0;									// _0C
    virtual int GetHeight() const = 0;									// _10
    virtual int GetAscent() const = 0;									// _14
    virtual int GetDescent() const = 0;									// _18
    virtual int GetBaselinePos() const = 0;								// _1C
    virtual int GetCellHeight() const = 0;								// _20
    virtual int GetCellWidth() const = 0;								// _24
    virtual int GetMaxCharWidth() const = 0;							// _28
    virtual Type GetType() const = 0;									// _2C
    virtual GXTexFmt GetTextureFormat() const = 0;						// _30
    virtual int GetLineFeed() const = 0;								// _34
    virtual CharWidths GetDefaultCharWidths() const = 0;				// _38
    virtual void SetDefaultCharWidths(const CharWidths& widths) = 0;	// _3C
    virtual bool SetAlternateChar(u16 c) = 0;							// _40
    virtual void SetLineFeed(int linefeed) = 0;							// _44
    virtual int GetCharWidth(u16 c) const = 0;							// _48
    virtual CharWidths GetCharWidths(u16 c) const = 0;					// _4C
    virtual void GetGlyph(Glyph* glyph, u16 c) const = 0;				// _50
    virtual FontEncoding GetEncoding() const = 0;						// _54


	void InitReaderFunc(FontEncoding encoding);


	CharStrmReader GetCharStrmReader() const
	{
		CharStrmReader reader(mReaderFunc);
		return reader;
	}

private:
	// _00     = VTBL
	CharStrmReader::ReadFunc mReaderFunc;	// _04
}; 
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_FONT_H
