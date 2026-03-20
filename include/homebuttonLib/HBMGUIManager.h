#ifndef REVOSDK_HBM_GUI_MANAGER_H
#define REVOSDK_HBM_GUI_MANAGER_H

#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/ut/list.h"
#include <RevoSDK/kpad.h>
#include <RevoSDK/mem.h>
#include <types.h>


namespace nw4hbm { 
namespace lyt { 
class DrawInfo;
class Layout; 
} // namespace lyt
} // namespace nw4hbm

namespace homebutton { 
namespace gui {
class Manager;
class PaneComponent;

class Interface {
public:
	
	virtual void create() {}              // _08
    virtual void init() {}                // _0C
    virtual void calc() {}                // _10
    virtual void draw(Mtx&) {} 			  // _14
    virtual void draw() {}                // _18
    virtual ~Interface() {}               // _1C
};

class EventHandler {
public:
	typedef u32 EventType;
	enum EventType_et
	{
		TrigEvent,
		PointEvent,
		LeftEvent,
		Event3,			// unsure. some sort of MoveEvent maybe? idk
		Event4,			// Completely unused. Old removed event?
		ReleaseEvent	// Possibly; see Manager::update
	};

	virtual void onEvent(u32, EventType, void *) {} 						// _08
	virtual void setManager(Manager *pManager) { mpManager = pManager; } 	// _0C

protected:
	// _00     = VTBL
	Manager	*mpManager;	// _04
};

class Component : public Interface {
		// NOTE the misspelling of triggerTarget as triggerTarger
public:
	Component(u32 uID)
		: mbDragging(false)
		, muDraggingButton(0)
		, muID(uID)
		, mbTriggerTarger(false)
		, mpManager(nullptr)
	{
		init();
	}

	virtual void init()													// _0C
	{
		mbDragging = false;

		for (int i = 0; i < (int)ARRAY_SIZE(mabPointed); ++i)
			mabPointed[i] = false;
	}
	virtual u32 getID() { return muID; }								// _20
	virtual int isPointed(int n) { return mabPointed[n]; } 				// _24
	virtual void setPointed(int n, bool b) { mabPointed[n] = b; } 		// _28
	virtual void onPoint() {}											// _2C
	virtual void offPoint() {}  										// _30
	virtual void onDrag(f32, f32) {}									// _34
	virtual void onMove(f32, f32) {} 									// _38
	virtual void onTrig(u32 uFlag, Vec &vec) 							// _3C
	{
		if (uFlag & muDraggingButton)
		{
			mDragStartPos = vec;
			mbDragging = true;
		}
	}
	virtual void setDraggingButton(u32 uDraggingButton) 				// _40
	{
		muDraggingButton = uDraggingButton;
	}

	virtual bool update(int, KPADStatus const *, f32, f32, void *) 		// _44
	{
		return false;
	}
	virtual bool update(int i, f32 x, f32 y, u32 uTrigFlag,
		                u32 uHoldFlag, u32 uReleaseFlag,
		                void *pData); 									// _48
	virtual bool isTriggerTarger() { return mbTriggerTarger; }			// _4C
	virtual void setTriggerTarget(bool bTriggerTarget)					// _50
	{
		mbTriggerTarger = bTriggerTarget;
	}
	virtual void setManager(Manager *pManager)							// _54
	{ 
		mpManager = pManager; 
	}
	virtual bool isVisible() { return true; } 							// _58
	virtual bool contain(f32 x_, f32 y_) = 0;							// _5C

protected: 
	// _00     = VTBL
	// _00-_04 = Interface
	bool mabPointed[8];			// _04
	Vec mDragStartPos;			// _0C
	bool mbDragging;			// _18
	u32	muDraggingButton;		// _1C
	u32 muID;					// _20
	bool mbTriggerTarger;		// _24
	Manager	*mpManager;			// _28
}; 

class Manager : public Interface {
private:
	struct IDToComponent {
		IDToComponent(u32 uID, Component *pComponent)
			: muID(uID)
			, mpComponent(pComponent)
		{
		}

		u32 muID;					// _00
		Component *mpComponent;		// _04
		nw4hbm::ut::Link mLink;		// _08
	};

public:
	Manager(EventHandler *pEventHandler, MEMAllocator *pAllocator)
		: mpEventHandler (pEventHandler)
		, mpAllocator (pAllocator)
	{
		if (mpEventHandler)
			mpEventHandler->setManager(this);

		nw4hbm::ut::List_Init(&mIDToComponent,
			                offsetof(IDToComponent, mLink));
	}

	virtual ~Manager();  												// _1C
	
	virtual void init();												// _0C
	virtual void calc();												// _10
	virtual void draw();												// _14
	virtual void addComponent(Component *pComponent); 					// _20
	virtual void delComponent(Component *pComponent); 					// _24
	virtual Component *getComponent(u32 uID); 							// _28
	virtual bool update(int, KPADStatus const *, f32, f32, void *)  	// _2C
	{
		return false;
	}
	virtual bool update(int i, f32 x, f32 y, u32 uTrigFlag,
		                    u32 uHoldFlag, u32 uReleaseFlag,
		                    void *pData);								// _30
	virtual void onEvent(u32 uID, EventHandler::EventType uEvent,
		                     void *pData)								// _34
	{
		if (mpEventHandler)
			mpEventHandler->onEvent(uID, uEvent, pData);
	}
	virtual void setAllComponentTriggerTarget(bool b);					// _38
	virtual void setEventHandler(EventHandler *pEventHandler)			// _3C
	{
		mpEventHandler = pEventHandler;

		if (mpEventHandler)
			mpEventHandler->setManager(this);
	}

protected:
	// _00     = VTBL
	// _00-_04 = Interface	
	EventHandler *mpEventHandler;		// _04
	nw4hbm::ut::List mIDToComponent;	// _08
	MEMAllocator *mpAllocator;			// _14
};

class PaneManager : public Manager {
private:
	struct PaneToComponent {	
		PaneToComponent(nw4hbm::lyt::Pane *pPane, PaneComponent *pComponent)
			: mpPane(pPane)
			, mpComponent(pComponent)
		{
		}

		nw4hbm::lyt::Pane *mpPane;		// _00
		PaneComponent *mpComponent;		// _04
		nw4hbm::ut::Link mLink;			// _08
	};

public:
	PaneManager(EventHandler *pEventHandler,
		            nw4hbm::lyt::DrawInfo const *pDrawInfo,
		            MEMAllocator *pAllocator)
		: Manager(pEventHandler, pAllocator)
		, mpDrawInfo(pDrawInfo)
	{
		nw4hbm::ut::List_Init(&mPaneToComponent, offsetof(PaneToComponent, mLink));
	}

	virtual ~PaneManager(); 													// _1C
	virtual void createLayoutScene(nw4hbm::lyt::Layout const &rLayout); 		// _40
	virtual void addLayoutScene(nw4hbm::lyt::Layout const &rLayout); 			// _44
	virtual void delLayoutScene(nw4hbm::lyt::Layout const &rLayout); 			// _48
	virtual PaneComponent *getPaneComponentByPane(nw4hbm::lyt::Pane *pPane); 	// _4C
	virtual nw4hbm::lyt::DrawInfo const *getDrawInfo() 							// _50
	{
		return mpDrawInfo;
	}
	virtual void setDrawInfo(nw4hbm::lyt::DrawInfo const *pDrawInfo) 			// _54
	{
		mpDrawInfo = pDrawInfo;
	}
	virtual void setAllBoundingBoxComponentTriggerTarget(bool b); 				// _58
	virtual void walkInChildren(nw4hbm::lyt::Pane::LinkList &rPaneList); 		// _5C
	virtual void walkInChildrenDel(nw4hbm::lyt::Pane::LinkList &rPaneList); 	// _60

private:
	static u32 suIDCounter;
	
	// _00     = VTBL
	// _00-_18 = Manager
	nw4hbm::ut::List mPaneToComponent;			// _18
	nw4hbm::lyt::DrawInfo const *mpDrawInfo;	// _24
	u16 muNumPoint;								// _28
};

class PaneComponent : public Component {
public:
	PaneComponent(u32 uID)
		: Component(uID)
		, mpPane(nullptr)
		{
		}

	virtual void draw(); 												// _18
	virtual bool isVisible(); 											// _58
	virtual bool contain(f32 x_, f32 y_); 								// _5C
	virtual void setPane(nw4hbm::lyt::Pane *pPane) { mpPane = pPane; } 	// _60
	virtual nw4hbm::lyt::Pane *getPane() { return mpPane; } 			// _64

private:
	// _00     = VTBL
	// _00-_2C = Component
	nw4hbm::lyt::Pane *mpPane;	// _2C
};
} // namespace gui
} // namespace homebutton

#endif // REVOSDK_HBM_GUI_MANAGER_H
