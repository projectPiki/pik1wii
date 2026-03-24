#ifndef NW4R_LYT_TEXTBOX_H
#define NW4R_LYT_TEXTBOX_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_types.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

// Forward declarations
struct ResBlockSet;

/******************************************************************************
 *
 * TextColor
 *
 ******************************************************************************/
enum TextColor {
    TEXTCOLOR_TOP,
    TEXTCOLOR_BOTTOM,

    TEXTCOLOR_MAX
};

namespace res {

/******************************************************************************
 *
 * TXT1 binary layout
 *
 ******************************************************************************/
struct TextBox : Pane {
    static const u32 SIGNATURE = FOURCC('t', 'x', 't', '1');

    u16 textBufBytes;             	     // _4C
    u16 textStrBytes;            	     // _4E
    u16 materialIdx;              	     // _50
    u16 fontIdx;                  	     // _52
    u8 textPosition;              	     // _54
    u8 textAlignment;             	     // _55
    u8 PADDING_0x56[0x58 - 0x56]; 	     // _56
    u32 textStrOffset;            	     // _58
    u32 textCols[TEXTCOLOR_MAX];  	     // _5C
    Size fontSize;                	     // _64
    f32 charSpace;                	     // _6C
    f32 lineSpace;                	     // _70
};

} // namespace res

/******************************************************************************
 *
 * TextBox
 *
 ******************************************************************************/
class TextBox : public Pane {
public:
    NW4R_UT_RTTI_DECL(TextBox);

public:
    TextBox(const res::TextBox* pRes, const ResBlockSet& rBlockSet);
    
	virtual ~TextBox(); 									   	     // _08
    virtual void DrawSelf(const DrawInfo& rInfo); 			   	     // _18
    virtual ut::Color GetVtxColor(u32 idx) const;  			   	     // _24
    virtual void SetVtxColor(u32 idx, ut::Color color);			   	 // _28
    virtual u8 GetVtxColorElement(u32 idx) const; 			   	     // _34
    virtual void SetVtxColorElement(u32 idx, u8 value);			     // _38
	virtual void LoadMtx(const DrawInfo &drawInfo);    			   	 // _70
    virtual void AllocStringBuffer(u16 len); 			   	         // _74
    virtual void FreeStringBuffer();         			   	   	     // _78
    virtual u16 SetString(const wchar_t* pStr, u16 pos = 0);         // _7C
    virtual u16 SetString(const wchar_t* pStr, u16 pos, u16 len);    // _80


    u16 GetStringBufferLength() const;
	ut::Rect GetTextDrawRect(const DrawInfo& rInfo) const;
    ut::Rect GetTextDrawRect(ut::TextWriterBase<wchar_t> *pWriter) const;

    const wchar_t* GetString() const {
        return mTextBuf;
    }
    const wchar_t* GetStringBuffer() const {
        return mTextBuf;
    }

    const ut::Font* GetFont() const;
    void SetFont(const ut::Font* pFont);
	u16 SetStringImpl(const wchar_t *str, u16 dstIdx, u32 strLen);

    ut::Color GetTextColor(u32 idx) const {
        return mTextColors[idx];
    }
    void SetTextColor(u32 idx, ut::Color color) {
        mTextColors[idx] = color;
    }

    const Size& GetFontSize() const {
        return mFontSize;
    }
    void SetFontSize(const Size& rFontSize) {
        mFontSize = rFontSize;
    }

    f32 GetLineSpace() const {
        return mLineSpace;
    }
    void SetLineSpace(f32 space) {
        mLineSpace = space;
    }

    f32 GetCharSpace() const {
        return mCharSpace;
    }
    void SetCharSpace(f32 space) {
        mCharSpace = space;
    }

    ut::TagProcessorBase<wchar_t> *GetTagProcessor() const {
        return mpTagProcessor;
    }
    void SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor) {
        mpTagProcessor = pTagProcessor;
    }

    u8 GetTextPositionH() const {
        return detail::GetHorizontalPosition(mTextPosition);
    }
    void SetTextPositionH(u8 value) {
        detail::SetHorizontalPosition(&mTextPosition, value);
    }

    u8 GetTextPositionV() const {
        return detail::GetVerticalPosition(mTextPosition);
    }
    void SetTextPositionV(u8 value) {
        detail::SetVerticalPosition(&mTextPosition, value);
    }

    f32 GetTextMagH() const;
    f32 GetTextMagV() const;
	f32 GetTextAlignMag() const;
    u32 MakeDrawFlag() const;

protected:
    wchar_t* mTextBuf;                   			   // _D8
    ut::Color mTextColors[TEXTCOLOR_MAX]; 			   // _DC
    const ut::Font* mpFont; 						   // _E4
    Size mFontSize;         						   // _E8
    f32 mLineSpace;         			  			   // _F0
    f32 mCharSpace;         			   			   // _F4
    ut::TagProcessorBase<wchar_t> *mpTagProcessor;	   // _F8
    u16 mTextBufBytes;                    			   // _FC
    u16 mTextLen;                     				   // _FE
    u8 mTextPosition;                   			   // _100
    struct {
        u8 bAllocFont : 1;
		u8 textAlignment : 2;                      
    } mBits; 										   // _101

private:
    void Init(u16 len);
};

} // namespace lyt
} // namespace nw4r

#endif
