#ifndef NW4HBM_LYT_TEXT_BOX_H
#define NW4HBM_LYT_TEXT_BOX_H

#include "homebuttonLib/nw4hbm/macros.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/ut/Color.h"
#include "homebuttonLib/nw4hbm/ut/Rect.h"
#include <types.h>


namespace nw4hbm { 
namespace ut { 
class Font;
template <typename> class TagProcessorBase;
template <typename> class TextWriterBase;

namespace detail { 
class RuntimeTypeInfo; 

}
}

namespace lyt { 
struct ResBlockSet; 

namespace res {	
struct Font{
	u32	nameStrOffset;	// _00
	u8	type;			// _04
}; 

static u32 const SIGNATURE_TEXT_BOX_BLOCK =	NW4HBM_FOUR_CHAR('t', 'x', 't', '1');

struct TextBox : public Pane {
	
	// _00-_4C = Pane
	u16	textBufBytes;	// _4C
	u16	textStrBytes;	// _4E
	u16	materialIdx;	// _50
	u16	fontIdx;		// _52
	u8	textPosition;	// _54
	u32	textStrOffset;	// _58
	u32	textCols[2];	// _5c
	Size fontSize;		// _64
	f32	charSpace;		// _6C
	f32	lineSpace;		// _70
};
} // namespace res


class TextBox : public Pane{
public:
	TextBox(u16 allocStrLen);
	TextBox(u16 allocStrLen, wchar_t const *str, ut::Font const *pFont);
	TextBox(u16 allocStrLen, wchar_t const *str, u16 strLen, ut::Font const *pFont);
	TextBox(res::TextBox const *pBlock, ResBlockSet const &resBlockSet);
	
	virtual ~TextBox(); 													// _08
	virtual ut::detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const 	// _0C
	{
		return &typeInfo;
	}
	virtual void DrawSelf(DrawInfo const &drawInfo); 						// _18
	virtual ut::Color GetVtxColor(u32 idx) const; 							// _24
	virtual void SetVtxColor(u32 idx, ut::Color value); 					// _28
	virtual u8 GetVtxColorElement(u32 idx) const; 							// _34
	virtual void SetVtxColorElement(u32 idx, u8 value); 					// _38
	virtual void AllocStringBuffer(u16 minLen); 							// _64
	virtual void FreeStringBuffer(); 										// _68
	virtual u16 SetString(wchar_t const *str, u16 dstIdx); 					// _6C
	virtual u16 SetString(wchar_t const *str, u16 dstIdx, u16 strLen); 		// _70

	ut::Color GetTextColor(u32 type) const { return mTextColors[type]; }
	Size GetFontSize() const { return mFontSize; }

	void SetFontSize(Size const &fontSize) { mFontSize = fontSize; }
	void SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor)
	{
		mpTagProcessor = pTagProcessor;
	}

	void SetTextColor(u32 type, ut::Color value)
	{
		mTextColors[type] = value;
	}

	ut::Font const *GetFont() const;
	u16 GetStringBufferLength() const;
	ut::Rect GetTextDrawRect(ut::TextWriterBase<wchar_t> *pWriter) const;
	f32 GetTextMagH() const;
	f32 GetTextMagV() const;
	u8 GetTextPositionH() const
	{
		return detail::GetHorizontalPosition(mTextPosition);
	}

	u8 GetTextPositionV() const
	{
		return detail::GetVerticalPosition(mTextPosition);
	}

	ut::Rect GetTextDrawRect(DrawInfo const &drawInfo) const;

	void SetFont(ut::Font const *pFont);
	void SetTextPositionH(u8 pos)
	{
		detail::SetHorizontalPosition(&mTextPosition, pos);
	}

	void SetTextPositionV(u8 pos)
	{
		detail::SetVerticalPosition(&mTextPosition, pos);
	}

	void Init(u16 allocStrLen);

public:
	static ut::detail::RuntimeTypeInfo const typeInfo;

private:
	// _00-_D0 = Pane
	wchar_t	*mTextBuf;									// _D4
	ut::Color mTextColors[2];							// _D8
	ut::Font const *mpFont;								// _E0
	Size mFontSize;										// _E4
	f32	mLineSpace;										// _EC
	f32	mCharSpace;										// _F0
	ut::TagProcessorBase<wchar_t> *mpTagProcessor;		// _F4
	u16 mTextBufBytes;									// _F8
	u16 mTextLen;										// _FA
	u8 mTextPosition;									// _FC
	struct {
		u8 allocFont : 1;
	} mTextBoxFlag; 									// _FD
};
} // namespace lyt
} // namespace nw4hbm

#endif // NW4HBM_LYT_TEXT_BOX_H
