#ifndef NW4HBM_LYT_RESOURCE_ACCESSOR_H
#define NW4HBM_LYT_RESOURCE_ACCESSOR_H

#include "homebuttonLib/nw4hbm/macros.h"
#include "homebuttonLib/nw4hbm/ut/Font.h"
#include <types.h>

namespace nw4hbm {
namespace lyt {

class ResourceAccessor {
public:
	ResourceAccessor();
		
	virtual ~ResourceAccessor();
	virtual void *GetResource(u32 resType, char const *name,
		                           u32 *pSize) = 0;
	virtual ut::Font *GetFont(char const *name);


public:
	static u32 const SIGNATURE_FONT = NW4HBM_FOUR_CHAR('f', 'o', 'n', 't');
	static u32 const SIGNATURE_TEXTURE_IMAGE =	NW4HBM_FOUR_CHAR('t', 'i', 'm', 'g');

private:
	// _00     = VTBL
};
} // namespace lyt
} // namespace nw4hbm

#endif // NW4HBM_LYT_RESOURCE_ACCESSOR_H
