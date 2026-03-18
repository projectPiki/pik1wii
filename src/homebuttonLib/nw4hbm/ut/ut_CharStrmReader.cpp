#include "homebuttonLib/nw4hbm/ut/CharStrmReader.h"
#include <types.h>

namespace nw4hbm { 
namespace ut { 
namespace {
	inline bool IsSJISLeadByte(byte_t c)
	{
		return (0x81 <= c && c < 0xA0) || 0xE0 <= c;
	}
} // namespace
} // namespace ut
} // namespace nw4hbm

namespace nw4hbm { 
namespace ut {

u16 CharStrmReader::ReadNextCharUTF8()
{
	byte2_t code;

	if ((GetChar<u8>(0) & 0x80) == 0x00)
	{
		// 1-byte UTF-8 sequence
		code = GetChar<u8>(0);
		StepStrm<u8>(1);
	}
	else if ((GetChar<u8>(0) & 0xE0) == 0xC0)
	{
		// 2-byte UTF-8 sequence
		code = (GetChar<u8>(0) & 0x1f) << 6 | (GetChar<u8>(1) & 0x3f);
		StepStrm<u8>(2);
	}
	else // if ((GetChar<u8>(0) & 0xF0) == 0xE0)
	{
		// 3-byte UTF-8 sequence

		code = (GetChar<u8>(0) & 0x1f) << 12
		     | (GetChar<u8>(1) & 0x3f) << 6 | (GetChar<u8>(2) & 0x3f);
		StepStrm<u8>(3);
	}

	return code;
}

u16 CharStrmReader::ReadNextCharUTF16()
{
	byte2_t code;

	code = GetChar<u16>(0);
	StepStrm<u16>(1);

	return code;
}

u16 CharStrmReader::ReadNextCharCP1252()
{
	byte2_t code;

	code = GetChar<u8>(0);
	StepStrm<u8>(1);

	return code;
}

u16 CharStrmReader::ReadNextCharSJIS()
{
	byte2_t code;

	if (IsSJISLeadByte(GetChar<u8>(0)))
	{
		code = GetChar<u8>(0) << 8 | GetChar<u8>(1);
		StepStrm<u8>(2);
	}
	else
	{
		code = GetChar<u8>(0);
		StepStrm<u8>(1);
	}

	return code;
}

} // namespace ut
} // namespace nw4hbm
