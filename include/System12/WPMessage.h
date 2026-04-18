#ifndef _SYSTEM12_WP_MESSAGE_H
#define _SYSTEM12_WP_MESSAGE_H

#include <egg/core/eggSingleton.h>
#include <nw4r/ut.h>


namespace System12 {
	
class WPMessage {
	
EGG_SINGLETON_DECL(WPMessage);

public:
	
	WPMessage();

	void initialize(const void*);
	static void AnalyzeTagWide(unsigned short code, const wchar_t* tag, unsigned char* pTagLength, unsigned long* pTagID, void** ppParam);
	
	virtual ~WPMessage();
	
	static const u16 TAG_MARK;

};

} // namespace System12

#endif
