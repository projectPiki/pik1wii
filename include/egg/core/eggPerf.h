#ifndef EGG_CORE_PERF_H
#define EGG_CORE_PERF_H

namespace EGG {

class PerformanceView {
public:
	virtual void measureBeginFrame()         = 0; // _08
	virtual void measureEndFrame()           = 0; // _0C
	virtual void measureBeginRender()        = 0; // _10
	virtual void measureEndRender()          = 0; // _14
	virtual void callbackDrawSync(u16 token) = 0; // _18
	virtual void unused()                    = 0; // _1C
	virtual void draw()                      = 0; // _20
	virtual void setVisible(bool visible)    = 0; // _24
	virtual bool isVisible()                 = 0; // _28
};

} // namespace EGG

#endif
