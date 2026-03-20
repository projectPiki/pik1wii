#ifndef NW4HBM_UT_CHAR_WRITER_H
#define NW4HBM_UT_CHAR_WRITER_H

#include "homebuttonLib/nw4hbm/math/types.h"
#include "homebuttonLib/nw4hbm/ut/Color.h"
#include <types.h>

namespace nw4hbm { 
namespace ut { 
class Font;
struct Glyph;

class CharWriter {
public:
    enum GradationMode {
        GRADMODE_NONE = 0,
        GRADMODE_H,
        GRADMODE_V,
		
        NUM_OF_GRADMODE
    };

private:
    struct ColorMapping {
        Color min; // _00 
        Color max; // _04
    };

    struct VertexColor {
        Color lu, ru; // _00 
        Color ld, rd; // _08 
    };

    struct TextureFilter {
    public:
        bool operator!=(const TextureFilter& rhs) const { return atSmall != rhs.atSmall || atLarge != rhs.atLarge; }

        GXTexFilter atSmall; // _00 
        GXTexFilter atLarge; // _04 
    };

    struct TextColor {
        Color start; 					// _00 
        Color end; 						// _04 
        GradationMode gradationMode; 	// _08 
    };

    struct LoadingTexture {
    public:
        bool operator!=(const LoadingTexture& rhs) const {
            return slot != rhs.slot || texture != rhs.texture || filter != rhs.filter;
        }

        void Reset() {
            slot = GX_TEXMAP_NULL;
            texture = nullptr;
        }

        GXTexMapID slot; 		// _00 
        void* texture; 			// _04 
        TextureFilter filter; 	// _08 
    };

public:
    CharWriter();
	~CharWriter();

	Color GetColorMappingMin() const;
	Color GetColorMappingMax() const;
	Color GetTextColor() const;
	Color GetGradationStartColor() const;
	Color GetGradationEndColor() const;
	GradationMode GetGradationMode() const;
	f32 GetScaleH() const;
	f32 GetScaleV() const;
	f32 GetCursorX() const;
	f32 GetCursorY() const;
	f32 GetCursorZ() const;
	bool IsLinearFilterEnableAtSmall() const;
	bool IsLinearFilterEnableAtLarge() const;
	u8 GetAlpha() const;
	Font const *GetFont() const;

	void SetColorMapping(Color min, Color max);
	void SetScale(f32 hScale, f32 vScale);
	void SetScale(f32 scale);
	void SetCursor(f32 x, f32 y);
	void SetCursor(f32 x, f32 y, f32 z);
	void MoveCursor(f32 dx, f32 dy);
	void MoveCursor(f32 dx, f32 dy, f32 dz);
	void SetCursorX(f32 x);
	void SetCursorY(f32 y);
	void SetCursorZ(f32 z);
	void MoveCursorX(f32 dx);
	void MoveCursorY(f32 dy);
	void MoveCursorZ(f32 dz);
	void EnableLinearFilter(bool atSmall, bool atLarge);
	void EnableFixedWidth(bool isFixed);
	void SetFixedWidth(f32 width);
	void SetFont(Font const &font);

	f32 GetFontWidth() const;
	f32 GetFontHeight() const;
	f32 GetFontAscent() const;
	f32 GetFontDescent() const;
	bool IsWidthFixed() const;
	f32 GetFixedWidth() const;

	void SetGradationMode(GradationMode mode);
	void SetTextColor(Color color);
	void SetTextColor(Color start, Color end);
	void SetFontSize(f32 width, f32 height);
	void SetFontSize(f32 height);
	void SetAlpha(u8 alpha);

	void SetupGX();
	void ResetColorMapping();
	void ResetTextureCache();
	f32 Print(u16 code);
	void DrawGlyph(Glyph const &glyph);
	void PrintGlyph(f32 x, f32 y, f32 z, Glyph const &glyph);
	void LoadTexture(Glyph const &glyph, GXTexMapID slot);
	void UpdateVertexColor();

private:
	static void SetupVertexFormat();

	static void SetupGXDefault();
	static void SetupGXWithColorMapping(Color min, Color max);
	static void SetupGXForI();
	static void SetupGXForRGBA();

    ColorMapping mColorMapping; 	// _00 
    VertexColor mVertexColor; 		// _08
    TextColor mTextColor; 			// _18
    math::VEC2 mScale; 				// _24
    math::VEC3 mCursorPos; 			// _2C
    TextureFilter mFilter;			// _38
    u8 padding_[2]; 				// _40 
    u8 mAlpha; 						// _42
    bool mIsWidthFixed; 			// _43 
    f32 mFixedWidth; 				// _44
    const Font* mFont; 				// _48

    static const u32 DEFAULT_COLOR_MAPPING_MIN = 0x00000000;
    static const u32 DEFAULT_COLOR_MAPPING_MAX = 0xFFFFFFFF;

private:
    static LoadingTexture mLoadingTexture;
};

} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_CHAR_WRITER_H
