#include "System12/WPWideTextWriter.h"
#include "egg/prim/eggAssert.h"

namespace System12 {
	
wchar_t* WPWideTextWriter::sStrExpBuffer = nullptr;
WPTagProcessor WPWideTextWriter::sDefaultTagProcessor;
	
void WPWideTextWriter::Initialize()
{
	sStrExpBuffer = new wchar_t[2048];
	EGG_ASSERT(17, sStrExpBuffer);
}

WPWideTextWriter::WPWideTextWriter()
{
	EGG_ASSERT_MSG(22, sStrExpBuffer, "Maybe, you forgot to Initialize().\n");
	
	SetBuffer(sStrExpBuffer, 2048);
	
	setRPSysTagProcessor(&sDefaultTagProcessor);
	reset();
	setWaitTimePerWord(0.0f);
}

WPWideTextWriter::~WPWideTextWriter()
{

}

void WPWideTextWriter::reset()
{
	_6C = 0;
	mPageIdx = 1;
	mRunning = true;
	_75 = false;
	setPrintOneStrokeMode(false);
	_7C = 0.0f;
}

void WPWideTextWriter::setWaitTimePerWord(f32 time)
{
	mWaitTimePerWord = time;
}

void WPWideTextWriter::setPrintOneStrokeMode(bool mode)
{
	mPrintOneStrokeMode = mode;
}

void WPWideTextWriter::calc(f32 time)
{
	if (mRunning) {
		return;
	}
	if (!_75) {
		return;
	}
	f32 rate = _7C + time;
	_7C = rate;
	if (mPrintOneStrokeMode) {
		_6C = 0x10000;
		return;
	}
	
	if (rate < mWaitTimePerWord) {
		return;
	}
	
	_7C = rate - mWaitTimePerWord;
	if (1 < _6C) {
		return;
	}
	_6C = _6C + 1;
}
	
}