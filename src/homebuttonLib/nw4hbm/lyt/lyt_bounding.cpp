#include "homebuttonLib/nw4hbm/lyt/bounding.h"
#include "homebuttonLib/nw4hbm/lyt/common.h"
#include "homebuttonLib/nw4hbm/lyt/drawInfo.h"
#include "homebuttonLib/nw4hbm/lyt/pane.h"
#include "homebuttonLib/nw4hbm/ut/Color.h"
#include "homebuttonLib/nw4hbm/ut/RuntimeTypeInfo.h"

namespace nw4hbm { 
namespace lyt {

ut::detail::RuntimeTypeInfo const Bounding::typeInfo(&Pane::typeInfo);

Bounding::Bounding(res::Bounding const *pBlock, ResBlockSet const &)
	: Pane(pBlock)
	{
	}

Bounding::~Bounding() {}

void Bounding::DrawSelf(DrawInfo const &drawInfo)
{
	if (drawInfo.IsDebugDrawMode())
	{
		LoadMtx(drawInfo);
		detail::DrawLine(GetVtxPos(), mSize, 0x00ff00ff); // green
	}
}

} // namespace lyt
} // namespace nw4hbm
