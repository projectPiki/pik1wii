#ifndef NW4HBM_UT_COLOR_H
#define NW4HBM_UT_COLOR_H

#include <RevoSDK/gx.h>
#include <types.h>


namespace nw4hbm { 
namespace ut {

struct Color : public GXColor {
public:
	Color() { operator =(0xffffffff); }
	Color(u32 color) { operator =(color); }
	Color(GXColor const &color) { operator =(color); }

	~Color() {}

	Color &operator =(u32 color)
	{
		ToU32ref() = color;
		return *this;
	}

	Color &operator =(GXColor const &color)
	{
		return *this = *reinterpret_cast<u32 const *>(&color);
	}

	operator u32() const { return ToU32ref(); }

	u32 &ToU32ref() { return *reinterpret_cast<u32 *>(this); }

	u32 const &ToU32ref() const
	{
		return *reinterpret_cast<u32 const *>(this);
	}

public:
	} ATTRIBUTE_ALIGN(4);
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_COLOR_H
