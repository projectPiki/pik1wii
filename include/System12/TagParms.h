#ifndef _SYSTEM12_TAGPARMS_H
#define _SYSTEM12_TAGPARMS_H

#include <egg/core/eggStream.h>
#include <egg/core/eggHeap.h>

namespace System12 {

struct TagParm;

struct TagParameters {
	TagParameters(char*);

	void read(EGG::Stream&);
	
	TagParm* mHead; 	// _00
	char* mName; 		// _04
};

struct TagParm {
	TagParm(TagParameters*, char*);


	virtual void doWrite(EGG::Stream&) { } 	// _00
	virtual void doRead(EGG::Stream&) { } 	// _04
	virtual void flushValue() { } 			// _08
	virtual void doDump() { } 				// _0C

	// _00 VTBL
	char* mName;    // _04
	TagParm* mNext; // _08
};

struct StringTagParm : public TagParm {
	StringTagParm(TagParameters*, char*);

	virtual void doWrite(EGG::Stream&); 	// _00
	virtual void doRead(EGG::Stream&);  	// _04
	virtual void flushValue();     			// _08
	virtual void doDump();         			// _0C

	char* mData; // _0C
};

template <typename T>
struct PrimTagParm : public TagParm {
	PrimTagParm(TagParameters* a, char* name)
	    : TagParm(a, name)
	{
	}

	virtual void doWrite(EGG::Stream&); 	// _00
	virtual void doRead(EGG::Stream&);  	// _04
	virtual void doDump();         			// _0C

	inline T& operator()() { return mData; }

	T mData; // _0C
};

} // namespace System12

#endif
