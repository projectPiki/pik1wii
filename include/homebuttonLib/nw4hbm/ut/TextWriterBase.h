#ifndef NW4HBM_UT_TEXT_WRITER_BASE_H
#define NW4HBM_UT_TEXT_WRITER_BASE_H

#include "homebuttonLib/nw4hbm/ut/CharWriter.h"

#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


namespace nw4hbm { 
namespace ut { 

struct Rect;
template <typename> class TagProcessorBase;


template <typename CharT>
class TextWriterBase : public CharWriter {
public:
	TextWriterBase();
	~TextWriterBase();
	
	void SetLineHeight(f32 lineHeight);
	f32 GetLineHeight() const;
	void SetLineSpace(f32 lineSpace);
	void SetCharSpace(f32 charSpace);
	f32 GetLineSpace() const;
	f32 GetCharSpace() const;
	void SetTabWidth(int tabWidth);
	int GetTabWidth() const;
	void SetDrawFlag(u32 flags);
	u32 GetDrawFlag() const;
	void SetTagProcessor(TagProcessorBase<CharT> *tagProcessor);
	void ResetTagProcessor();
	TagProcessorBase<CharT> &GetTagProcessor() const;
	f32 CalcFormatStringWidth(CharT const *format, ...) const;
	f32 CalcFormatStringHeight(CharT const *format, ...) const;
	void CalcFormatStringRect(Rect *pRect, CharT const *format, ...) const;
	void CalcVStringRect(Rect *pRect, CharT const *format, va_list args) const;
	f32 CalcStringWidth(CharT const *str, int length) const;
	f32 CalcStringWidth(CharT const *str) const;
	f32 CalcStringHeight(CharT const *str, int length) const;
	f32 CalcStringHeight(CharT const *str) const;
	void CalcStringRect(Rect *pRect, CharT const *str, int length) const;
	void CalcStringRect(Rect *pRect, CharT const *str) const;
	f32 Printf(CharT const *format, ...);
	f32 VPrintf(CharT const *format, va_list args);
	f32 Print(CharT const *str, int length);
	f32 Print(CharT const *str);
	static CharT *SetBuffer(CharT *buf, u32 size);
	static CharT *SetBuffer(u32 size);
	static u32 GetBufferSize();
	static CharT *GetBuffer();
	static int VSNPrintf(char *buffer, u32 count, char const *format, va_list arg)
	{
		return vsnprintf(buffer, count, format, arg);
	}
	static int VSNPrintf(wchar_t *buffer, u32 count, wchar_t const *format, va_list arg)
	{
		return vswprintf(buffer, count, format, arg);
	}

	static int StrLen(char const *str) { return strlen(str); }
	static int StrLen(wchar_t const *str) { return wcslen(str); }
	f32 CalcLineWidth(CharT const *str, int length);
	int CalcLineRectImpl(Rect *pRect, CharT const *str, int length);
	void CalcStringRectImpl(Rect *pRect, CharT const *str, int length);
	f32 PrintImpl(CharT const *str, int length);
	f32 AdjustCursor(f32 *xOrigin, f32 *yOrigin, CharT const *str, int length);

	bool IsDrawFlagSet(u32 mask, u32 flag) const
	{
		return (mDrawFlag & mask) == flag;
	}

private:
	static CharT *mFormatBuffer;
	static u32 mFormatBufferSize;
	static TagProcessorBase<CharT> mDefaultTagProcessor;


private:
	// _00-_4C = CharWriter
	f32 mCharSpace;                    		// _4C
	f32 mLineSpace;                     	// _50
	int mTabWidth;                     		// _54
	u32 mDrawFlag;                      // _58
	TagProcessorBase<CharT>	*mTagProcessor; // _5C
};

} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_TEXT_WRITER_BASE_H
