#include <nw4r/lyt.h>
#include <nw4r/ut.h>

#include <stl/wchar.h>

/******************************************************************************
 *
 * Utility functions
 *
 ******************************************************************************/
 
namespace nw4r {
namespace lyt {

NW4R_UT_RTTI_DEF_DERIVED(TextBox, Pane);

namespace {

void ReverseYAxis(math::MTX34 *pMtx) {
    // Based on impl in LoadMtx
    pMtx->_01 = -pMtx->_01;
    pMtx->_11 = -pMtx->_11;
    pMtx->_21 = -pMtx->_21;
}

inline u8 ClampColor(s16 value) {
    return value < 0 ? 0 : (value > 255 ? 255 : value);
}

ut::Color GetColor(const GXColorS10& rColor16) {
    GXColor color8 = {ClampColor(rColor16.r), ClampColor(rColor16.g),
                      ClampColor(rColor16.b), ClampColor(rColor16.a)};

    return ut::Color(color8);
}

template <typename T>
void CalcStringRect(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth);
template <typename T>
int CalcLineStrNum(
    f32 *pWidth, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
);
template <typename T>
int CalcLineRectImpl(
    ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
);
template <typename T>
void CalcStringRectImpl(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth);

template <typename T>
void CalcStringRect(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth) {
    ut::TextWriterBase<T> myCopy = *pTextWriter;
    CalcStringRectImpl<T>(pRect, &myCopy, str, length, maxWidth);
}

template <typename T>
int CalcLineStrNum(
    f32 *pWidth, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
) {
    ut::Rect rect;
    ut::TextWriterBase<T> myCopy = *pTextWriter;
    myCopy.SetCursor(0.0f, 0.0f);
    int ret = CalcLineRectImpl(&rect, &myCopy, str, length, maxWidth, pbOver);

    *pWidth = rect.GetWidth();
    return ret;
}

template <typename T>
int CalcLineRectImpl(
    ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth, bool *pbOver
) {
    ut::PrintContext<T> context = {pTextWriter, str, 0.0f, 0.0f, 0};
    f32 x = 0.0f;

    const ut::Font *font = pTextWriter->GetFont();
    bool bCharSpace = false;
    ut::CharStrmReader reader = font->GetCharStrmReader();

    const T *prStrPos = (const T *)reader.GetCurrentPos();

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = ut::Min(0.0f, pTextWriter->GetLineHeight());
    pRect->bottom = ut::Max(0.0f, pTextWriter->GetLineHeight());

    *pbOver = false;

    reader.Set(str);
    ut::Rect prMaxRect = *pRect;
    u16 code = reader.Next();
    while (((const T *)reader.GetCurrentPos() - str) <= length) {
        if (code < ' ') {
            ut::TagProcessorBase<T>::Operation operation;
            ut::Rect rect(x, 0.0f, 0.0f, 0.0f);

            context.str = (const T *)reader.GetCurrentPos();
            context.flags = !bCharSpace;

            pTextWriter->SetCursorX(x);
            operation = pTextWriter->GetTagProcessor()->CalcRect(&rect, code, &context);
            reader.Set(context.str);

            pRect->left = ut::Min(pRect->left, rect.left);
            pRect->top = ut::Min(pRect->top, rect.top);
            pRect->right = ut::Max(pRect->right, rect.right);
            pRect->bottom = ut::Max(pRect->bottom, rect.bottom);

            x = pTextWriter->GetCursorX();

            if (pRect->GetWidth() > maxWidth) {
                *pbOver = true;
                break;
            }
            if (operation == ut::TagProcessorBase<T>::OPERATION_END_DRAW) {
                return length;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NO_CHAR_SPACE) {
                bCharSpace = false;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_CHAR_SPACE) {
                bCharSpace = true;
            } else if (operation == ut::TagProcessorBase<T>::OPERATION_NEXT_LINE) {
                break;
            }

        } else {
            if (bCharSpace) {
                x += pTextWriter->GetCharSpace();
            }
            bCharSpace = true;
            if (pTextWriter->IsWidthFixed()) {
                x += pTextWriter->GetFixedWidth();
            } else {
                x += pTextWriter->GetFont()->GetCharWidth(code) * pTextWriter->GetScaleH();
            }
            pRect->left = ut::Min(pRect->left, x);
            pRect->right = ut::Max(pRect->right, x);

            if (pRect->GetWidth() > maxWidth) {
                *pbOver = true;
                break;
            }
        }
        prStrPos = (const T *)reader.GetCurrentPos();
        code = reader.Next();
        prMaxRect = *pRect;
    }

    if (*pbOver) {
        if (prStrPos) {
            *pRect = prMaxRect;
            return (prStrPos - str);
        }
    }
    return ((const T *)reader.GetCurrentPos() - str);
}

template <typename T>
void CalcStringRectImpl(ut::Rect *pRect, ut::TextWriterBase<T> *pTextWriter, const T *str, int length, f32 maxWidth) {
    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = 0.0f;
    pRect->bottom = 0.0f;
    pTextWriter->SetCursor(0.0f, 0.0f);
    int remain = length;
    const T *pos = str;
    do {
        ut::Rect rect;
        bool bOver;
        int read = CalcLineRectImpl(&rect, pTextWriter, pos, remain, maxWidth, &bOver);
		if (bOver) {
            CalcLineRectImpl(&rect, pTextWriter, L"\n", 1, maxWidth, &bOver);
        }
        pos += read;
        remain -= read;
        pRect->left = ut::Min(pRect->left, rect.left);
        pRect->top = ut::Min(pRect->top, rect.top);
        pRect->right = ut::Max(pRect->right, rect.right);
        pRect->bottom = ut::Max(pRect->bottom, rect.bottom);
    } while (remain > 0);
}

} // namespace





/******************************************************************************
 *
 * TextBox
 *
 ******************************************************************************/
TextBox::TextBox(const res::TextBox* pRes, const ResBlockSet& rBlockSet)
    : Pane(pRes) {

    u16 len = pRes->textBufBytes / sizeof(wchar_t);
    if (len > 0) {
        len--;
    }

    Init(len);

    if (pRes->textStrBytes >= sizeof(wchar_t) && mTextBuf != nullptr) {
        const wchar_t* const pResStr =
            detail::ConvertOffsToPtr<wchar_t>(pRes, pRes->textStrOffset);

        u16 resLen = pRes->textStrBytes / sizeof(wchar_t) - 1;
        SetString(pResStr, 0, resLen);
    }

    for (int i = 0; i < TEXTCOLOR_MAX; i++) {
        mTextColors[i] = pRes->textCols[i];
    }

    mFontSize = pRes->fontSize;
    mTextPosition = pRes->textPosition;
	mBits.textAlignment = pRes->textAlignment;
    mCharSpace = pRes->charSpace;
    mLineSpace = pRes->lineSpace;

    const res::Font* const pResFonts = detail::ConvertOffsToPtr<res::Font>(
        rBlockSet.pFontList, sizeof(res::FontList));

    const char* const pFontName = detail::ConvertOffsToPtr<char>(
        pResFonts, pResFonts[pRes->fontIdx].nameStrOffset);

    ut::Font* pFont = rBlockSet.pResAccessor->GetFont(pFontName);

    if (pFont != nullptr) {
        mpFont = pFont;
    } else {
        void* pFontBinary = rBlockSet.pResAccessor->GetResource(
            ArcResourceAccessor::RES_TYPE_FONT, pFontName, nullptr);

        if (pFontBinary != nullptr) {
            ut::ResFont *pResFont = Layout::NewObj<ut::ResFont>();
            if (pResFont) {
                pResFont->SetResource(pFontBinary);

                mpFont = pResFont;
                mBits.bAllocFont = true;
            }
        }
    }

    const u32 *matOffsTbl = detail::ConvertOffsToPtr<u32>(rBlockSet.pMaterialList, sizeof(res::MaterialList));
    const res::Material *pResMaterial =
        detail::ConvertOffsToPtr<res::Material>(rBlockSet.pMaterialList, matOffsTbl[pRes->materialIdx]);
    mpMaterial = Layout::NewObj<Material>(pResMaterial, rBlockSet);

}

void TextBox::Init(u16 len) {
    mTextBuf = nullptr;
    mTextBufBytes = 0;
    mTextLen = 0;

    mpFont = nullptr;
    mFontSize = Size(0.0f, 0.0f);

    SetTextPositionH(HORIZONTALPOSITION_CENTER);
    SetTextPositionV(VERTICALPOSITION_CENTER);

    mLineSpace = 0.0f;
    mCharSpace = 0.0f;

    mpTagProcessor = nullptr;

    memset(&mBits, 0, sizeof(mBits));

    if (len > 0) {
        AllocStringBuffer(len);
    }
}

TextBox::~TextBox() {
    SetFont(nullptr);

    if (mpMaterial != nullptr && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
        mpMaterial = nullptr;
    }

    FreeStringBuffer();
}

ut::Color TextBox::GetVtxColor(u32 idx) const {
    return GetTextColor(idx / TEXTCOLOR_MAX);
}

void TextBox::SetVtxColor(u32 idx, ut::Color color) {
    SetTextColor(idx / TEXTCOLOR_MAX, color);
}

u8 TextBox::GetVtxColorElement(u32 idx) const {
    return reinterpret_cast<const u8*>(
        &mTextColors[idx / (4 * TEXTCOLOR_MAX)])[idx % 4];
}

void TextBox::SetVtxColorElement(u32 idx, u8 value) {
    reinterpret_cast<u8*>(&mTextColors[idx / (4 * TEXTCOLOR_MAX)])[idx % 4] =
        value;
}

void TextBox::DrawSelf(const DrawInfo& rInfo) {
    if (mTextBuf == nullptr || mpFont == nullptr || mpMaterial == nullptr) {
        return;
    }

    LoadMtx(rInfo);

    ut::TextWriterBase<wchar_t> writer;
    ut::Rect rect = GetTextDrawRect(&writer);

    ut::Color top =
        detail::MultipleAlpha(mTextColors[TEXTCOLOR_TOP], mGlbAlpha);
    ut::Color bottom =
        detail::MultipleAlpha(mTextColors[TEXTCOLOR_BOTTOM], mGlbAlpha);

    writer.SetGradationMode(top != bottom ? ut::CharWriter::GRADMODE_V
                                          : ut::CharWriter::GRADMODE_NONE);
    writer.SetTextColor(top, bottom);

    ut::Color min = GetColor(mpMaterial->GetTevColor(TEVCOLOR_REG0));
    ut::Color max = GetColor(mpMaterial->GetTevColor(TEVCOLOR_REG1));

    writer.SetColorMapping(min, max);
    writer.SetupGX();
	
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    f32 hMag = GetTextAlignMag();

    const wchar_t *strPos = mTextBuf;
    writer.SetCursor(rect.left, rect.top);
	
	f32 texWidth = rect.GetWidth();
    int remain = mTextLen;

    while (remain > 0) {
        bool bOver;
        f32 lineWidth;
        int lineStrNum = CalcLineStrNum(&lineWidth, &writer, strPos, remain, mSize.width, &bOver);
        f32 textPosX = rect.left + hMag * (texWidth - lineWidth);
        writer.SetCursorX(textPosX);
        writer.PrintMutable(strPos, lineStrNum);
        if (bOver) {
            writer.PrintMutable(L"\n", 1);
        }
        strPos += lineStrNum;
        remain -= lineStrNum;
    }
}

u16 TextBox::GetStringBufferLength() const {
    if (mTextBufBytes == 0) {
        return 0;
    }

    return mTextBufBytes / sizeof(wchar_t) - 1;
}

void TextBox::AllocStringBuffer(u16 len) {
    if (len == 0) {
        return;
    }

	u32 allocLen = len;
    u32 texBufBytes = (len + 1) * sizeof(wchar_t);

    if (texBufBytes > mTextBufBytes) {
        FreeStringBuffer();
        mTextBuf = Layout::NewArray<wchar_t>(allocLen + 1);

        if (mTextBuf != nullptr) {
            mTextBufBytes = texBufBytes;
        }
    }
}

void TextBox::FreeStringBuffer() {
    if (mTextBuf == nullptr) {
        return;
    }

    Layout::DeletePrimArray(mTextBuf);
    mTextBuf = nullptr;
    mTextBufBytes = 0;
}

u16 TextBox::SetString(const wchar_t* pStr, u16 pos) {
    return SetStringImpl(pStr, pos, wcslen(pStr));
}

u16 TextBox::SetString(const wchar_t *str, u16 dstIdx, u16 strLen) {
    return SetStringImpl(str, dstIdx, strLen);
}

u16 TextBox::SetStringImpl(const wchar_t* pStr, u16 pos, u32 len) {
    if (mTextBuf == nullptr) {
        return 0;
    }

    const u16 maxlen = GetStringBufferLength();
    if (pos >= maxlen) {
        return 0;
    }

    u32 chars = ut::Min<u32>(len, maxlen - pos);
    memcpy(&mTextBuf[pos], pStr, chars * sizeof(wchar_t));

    mTextLen = pos + chars;
    mTextBuf[mTextLen] = L'\0';

    return chars;
}

const ut::Font* TextBox::GetFont() const {
    return mpFont;
}

void TextBox::SetFont(const ut::Font* pFont) {
    if (mBits.bAllocFont) {
        Layout::DeleteObj(const_cast<ut::Font*>(mpFont));
        mBits.bAllocFont = false;
    }

    mpFont = pFont;

    if (mpFont != nullptr) {
        SetFontSize(Size(static_cast<f32>(mpFont->GetWidth()),
                         static_cast<f32>(mpFont->GetHeight())));
    } else {
        SetFontSize(Size(0.0f, 0.0f));
    }
}

void TextBox::LoadMtx(const DrawInfo &drawInfo) {
    math::MTX34 mtx;
    if (drawInfo.IsMultipleViewMtxOnDraw()) {
        PSMTXConcat(drawInfo.GetViewMtx(), mGlbMtx, mtx);
    } else {
        PSMTXCopy(mGlbMtx, mtx);
    }

    ReverseYAxis(&mtx);

    GXLoadPosMtxImm(mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

ut::Rect TextBox::GetTextDrawRect(ut::TextWriterBase<wchar_t> *pWriter) const {
    ut::Rect rect;

    pWriter->SetCursor(0.0f, 0.0f);

    pWriter->SetFont(*mpFont);
    pWriter->SetFontSize(mFontSize.width, mFontSize.height);

    pWriter->SetLineSpace(mLineSpace);
    pWriter->SetCharSpace(mCharSpace);

    if (mpTagProcessor != nullptr) {
        pWriter->SetTagProcessor(mpTagProcessor);
    }

    CalcStringRect(&rect, pWriter, mTextBuf, mTextLen, mSize.width);
	
	Size textSize(rect.GetWidth(), rect.GetHeight());
    math::VEC2 ltPos = GetVtxPos();

    // Adjust for Alignment (left, center, right)
    ltPos.x += (mSize.width - textSize.width) * GetTextMagH();
    ltPos.y -= (mSize.height - textSize.height) * GetTextMagV();

    // Apply the new pos
    rect.left = ltPos.x;
    rect.top = ltPos.y;
    rect.right = ltPos.x + textSize.width;
    rect.bottom = ltPos.y - textSize.height;

    return rect;
}

f32 TextBox::GetTextMagH() const {
    f32 mag = 0.0f;

    switch (GetTextPositionH()) {
    default:
    case HORIZONTALPOSITION_LEFT: {
        mag = 0.0f;
        break;
    }

    case HORIZONTALPOSITION_CENTER: {
        mag = 0.5f;
        break;
    }

    case HORIZONTALPOSITION_RIGHT: {
        mag = 1.0f;
        break;
    }
    }

    return mag;
}

f32 TextBox::GetTextMagV() const {
    f32 mag = 0.0f;

    switch (GetTextPositionV()) {
    default:
    case VERTICALPOSITION_TOP: {
        mag = 0.0f;
        break;
    }

    case VERTICALPOSITION_CENTER: {
        mag = 0.5f;
        break;
    }

    case VERTICALPOSITION_BOTTOM: {
        mag = 1.0f;
        break;
    }
    }

    return mag;
}

f32 TextBox::GetTextAlignMag() const {
    switch (mBits.textAlignment) {
        default: return GetTextMagH();
        case 0:  return 0.0f;
        case 2:  return 0.5f;
        case 3:  return 1.0f;
    }
}

u32 TextBox::MakeDrawFlag() const {
    u32 flag = 0;

    switch (GetTextPositionH()) {
    case HORIZONTALPOSITION_CENTER: {
        flag |= 1;
        break;
    }

    case HORIZONTALPOSITION_RIGHT: {
        flag |= 2;
        break;
    }

    default: {
        break;
    }
    }

    return flag;
}

} // namespace lyt
} // namespace nw4r