#include "System12/WPTagProcessor.h"
#include "System12/WPMessage.h"
#include "egg/prim/eggAssert.h"

namespace System12 {

nw4r::ut::Color WPTagProcessor::FONT_COLOR_TABLE[9] = {
	nw4r::ut::Color(nw4r::ut::Color::BLACK),
	nw4r::ut::Color(nw4r::ut::Color::RED),
	nw4r::ut::Color(nw4r::ut::Color::GREEN),
	nw4r::ut::Color(nw4r::ut::Color::BLUE),
	nw4r::ut::Color(nw4r::ut::Color::YELLOW),
	nw4r::ut::Color(nw4r::ut::Color::MAGENTA),
	nw4r::ut::Color(nw4r::ut::Color::WHITE),
	nw4r::ut::Color(104, 131, 230, 255),
	nw4r::ut::Color(230, 104, 172, 255),
};


WPTagProcessor::WPTagProcessor()
{
	_04 = 0.0f;
	_08 = nullptr;
}

WPTagProcessor::~WPTagProcessor()
{

}

nw4r::ut::TagProcessorBase<wchar_t>::Operation WPTagProcessor::Process(u16 ch, nw4r::ut::TagProcessorBase<wchar_t>::ContextType* pCtx)
{
	RuntimeProcess(nullptr, ch, pCtx);
}

nw4r::ut::TagProcessorBase<wchar_t>::Operation WPTagProcessor::CalcRect(nw4r::ut::Rect* pRect, u16 ch, nw4r::ut::TagProcessorBase<wchar_t>::ContextType* pCtx)
{
	RuntimeProcess(pRect, ch, pCtx);
}

nw4r::ut::TagProcessorBase<wchar_t>::Operation WPTagProcessor::RuntimeProcess(nw4r::ut::Rect* pRect, u16 ch, nw4r::ut::PrintContext<wchar_t>* pContext)
{
	EGG_ASSERT(217, pContext);
	if (WPMessage::TAG_MARK != ch) {
		if (pRect != nullptr){
			return nw4r::ut::TagProcessorBase<wchar_t>::CalcRect(pRect, ch, pContext);
		} else {
			return nw4r::ut::TagProcessorBase<wchar_t>::Process(ch, pContext);
		}
	} else {
		u8 cmdLen = 0;
		u32 cmd = 0;
		void *endPtr = nullptr;
		WPMessage::AnalyzeTagWide(ch, pContext->str, &cmdLen, &cmd, &endPtr);

		switch (cmd) {
			case 0xff0000:
				ProcessTagFontSize(pRect, pContext, cmdLen, endPtr);
				break;
			case 0xff0001:
				ProcessTagFontColor(pRect, pContext, cmdLen, endPtr);
				break;
			case 0xff0007:
				ProcessTagRuby(pRect, pContext, cmdLen, endPtr);
			default:
				break;
		}		
		pContext->str += cmdLen / 2;
		return nw4r::ut::TagProcessorBase<wchar_t>::OPERATION_DEFAULT;
		
	}
}

void WPTagProcessor::ProcessTagFontColor(nw4r::ut::Rect* rec, nw4r::ut::PrintContext<wchar_t>* pContext, u8 f, void* pParam)
{
	EGG_ASSERT(524, pContext);
	EGG_ASSERT(525, pParam);
	u8 colorIdx = *static_cast<const char*>(pParam);
	EGG_ASSERT(531, sizeof( FONT_COLOR_TABLE ) / sizeof( nw4r::ut::Color ) > colorIdx);
	pContext->writer->SetTextColor(FONT_COLOR_TABLE[colorIdx]);
}

void WPTagProcessor::ProcessTagFontSize(nw4r::ut::Rect* rec, nw4r::ut::PrintContext<wchar_t>* pContext, u8 f, void* pParam)
{
	EGG_ASSERT(554, pContext);
	EGG_ASSERT(555, pParam);
	f32 v1    = *static_cast<const u16*>(pParam) / 100.0f;
	f32 v2    = *static_cast<const u16*>(pParam) / 100.0f;
	pContext->writer->SetScale(v1, v2);
}
	
}