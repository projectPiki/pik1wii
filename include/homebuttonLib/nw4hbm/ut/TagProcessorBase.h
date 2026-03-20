#ifndef NW4HBM_UT_TAG_PROCESSOR_BASE_H
#define NW4HBM_UT_TAG_PROCESSOR_BASE_H

#include <types.h>

namespace nw4hbm {
namespace ut {
struct Rect;
template <typename> class TextWriterBase;

enum Operation
{
	OPERATION_DEFAULT,
	OPERATION_NO_CHAR_SPACE,
	OPERATION_CHAR_SPACE,
	OPERATION_NEXT_LINE,
	OPERATION_END_DRAW,

	NUM_OF_OPERATION
};

template <typename CharT>
struct PrintContext {
	TextWriterBase<CharT> *writer;	// _00
	CharT const *str;				// _04
	f32 xOrigin;					// _08
	f32 yOrigin;					// _0C
	u32 flags;					// _10
};

template <typename CharT>
class TagProcessorBase {
public:
	TagProcessorBase();
	
	virtual ~TagProcessorBase(); 														// _08
	virtual Operation Process(u16 code, PrintContext<CharT> *context); 					// _0C
	virtual Operation CalcRect(Rect *pRect, u16 code, PrintContext<CharT> *context); 	// _10
	
	void ProcessLinefeed(PrintContext<CharT> *context);
	void ProcessTab(PrintContext<CharT> *context);
};
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_TAG_PROCESSOR_BASE_H
