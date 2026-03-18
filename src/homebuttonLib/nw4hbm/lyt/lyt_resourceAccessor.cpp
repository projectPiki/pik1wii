#include "homebuttonLib/nw4hbm/lyt/resourceAccessor.h"
#include <types.h>

namespace nw4hbm { 
namespace ut { 
class Font; 
} // namespace ut

namespace lyt {

ResourceAccessor::~ResourceAccessor() {}

ResourceAccessor::ResourceAccessor() {}

ut::Font *ResourceAccessor::GetFont(char const *)
{
	return nullptr;
}

} // namespace lyt
} // namespace nw4hbm
