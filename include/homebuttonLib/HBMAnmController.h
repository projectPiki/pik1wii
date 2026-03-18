#ifndef REVOSDK_HBM_ANM_CONTROLLER_H
#define REVOSDK_HBM_ANM_CONTROLLER_H

#include "homebuttonLib/HBMFrameController.h"

// forward declarations
namespace nw4hbm { 
namespace lyt { 
class AnimTransform;
class Group; 
} // namespace lyt
} // namespace nw4hbm

namespace homebutton {
class GroupAnmController : public FrameController {
public:
	GroupAnmController();
		
	virtual ~GroupAnmController();

	void do_calc();

public:
	// _00     = VTBL
	// _00-_20 = FrameController
	nw4hbm::lyt::Group *mpGroup;				// _20
	nw4hbm::lyt::AnimTransform *mpAnimGroup;	// _24
};
} // namespace homebutton

#endif // REVOSDK_HBM_ANM_CONTROLLER_H
