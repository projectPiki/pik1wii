#ifndef NW4HBM_UT_RES_FONT_BASE_H
#define NW4HBM_UT_RES_FONT_BASE_H

#include "homebuttonLib/nw4hbm/ut/Font.h"
#include <RevoSDK/gx/GXEnum.h>
#include <types.h>

namespace nw4hbm { 
namespace ut { 
namespace detail {

class ResFontBase : public Font {
public:
	typedef u16 GlyphIndex;
	static GlyphIndex const GLYPH_INDEX_NOT_FOUND = 0xffff;

	ResFontBase();
	virtual ~ResFontBase();												// _08
    virtual int GetWidth() const;	   									// _0C
    virtual int GetHeight() const;										// _10
    virtual int GetAscent() const;										// _14
    virtual int GetDescent() const;										// _18
    virtual int GetBaselinePos() const;									// _1C
    virtual int GetCellHeight() const;									// _20
    virtual int GetCellWidth() const;									// _24
    virtual int GetMaxCharWidth() const;								// _28
    virtual Type GetType() const;										// _2C
    virtual GXTexFmt GetTextureFormat() const;							// _30
    virtual int GetLineFeed() const;									// _34
    virtual CharWidths GetDefaultCharWidths() const;					// _38
    virtual void SetDefaultCharWidths(const CharWidths& widths);		// _3C
    virtual bool SetAlternateChar(u16 c);								// _40
    virtual void SetLineFeed(int linefeed);								// _44
    virtual int GetCharWidth(u16 c) const;								// _48
    virtual CharWidths GetCharWidths(u16 c) const;						// _4C
    virtual void GetGlyph(Glyph* glyph, u16 c) const;					// _50
    virtual FontEncoding GetEncoding() const;							// _54

	GlyphIndex GetGlyphIndex(u16 c) const;
	CharWidths const &GetCharWidthsFromIndex(GlyphIndex index) const;
	CharWidths const &GetCharWidthsFromIndex(FontWidth const *pWidth,
		                                         GlyphIndex index) const;
	void GetGlyphFromIndex(Glyph *glyph, GlyphIndex index) const;
	bool IsManaging(void const *ptr) const { return mResource == ptr; }

	void SetResourceBuffer(void *pUserBuffer, FontInformation *pFontInfo);
	void *RemoveResourceBuffer();

	GlyphIndex FindGlyphIndex(u16 c) const;
	GlyphIndex FindGlyphIndex(FontCodeMap const *pMap, u16 c) const;


private:
	// _00     = VTBL
	// _00-_10 = Font
	void *mResource;			// _10
	FontInformation *mFontInfo;	// _14
}; 
} // namespace detail
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_RES_FONT_BASE_H
