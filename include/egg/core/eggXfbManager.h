#ifndef EGG_CORE_XFB_MANAGER_H
#define EGG_CORE_XFB_MANAGER_H

namespace EGG {

class Xfb;

class XfbManager {
public:
	Xfb* mNextXfb;    // _00, next xfb to process?
	Xfb* mToCopyXfb;  // _04, current xfb to use in copyEFB
	Xfb* mToShowXfb;  // _08, current xfb to use in setNextFrameBuffer
	u8 mNumXfbs;      // _0C, Total number of Xfbs ever attached
	u8 mNumXfbs_Copy; // _0D, Unsure of purpose yet, but showing wont proceed until its under 3
public:
	XfbManager()
	    : mNextXfb()
	    , mToCopyXfb()
	    , mToShowXfb()
	    , mNumXfbs()
	    , mNumXfbs_Copy()
	{
	}
	bool isRegisterd(Xfb& xfb) const;
	bool attach(Xfb* xfb);
	void copyEFB(bool);
	void setNextFrameBuffer();
};

} // namespace EGG

#endif
