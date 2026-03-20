#ifndef NW4HBM_LYT_ARC_RESOURCE_ACCESSOR_H
#define NW4HBM_LYT_ARC_RESOURCE_ACCESSOR_H

#include "homebuttonLib/nw4hbm/lyt/resourceAccessor.h"
#include "homebuttonLib/nw4hbm/ut/LinkList.h"
#include <RevoSDK/arc.h>
#include <types.h>


namespace nw4hbm { 
namespace lyt {
class FontRefLink {
public:
	typedef ut::LinkList<FontRefLink, 0> LinkList;

	FontRefLink();
	/* ~FontRefLink() = default; */

	char const *GetFontName() const { return mFontName; }
	ut::Font *GetFont() const { return mpFont; }

	void Set(char const *name, ut::Font *pFont);

	static u32 const FONT_NAME_SIZE = 128;

private:
	ut::LinkListNode mLink;				// _00
	char mFontName[FONT_NAME_SIZE];		// _08
	ut::Font *mpFont;					// _88
}; 

class ArcResourceLink {
public:
	typedef ut::LinkList<ArcResourceLink, 0> LinkList;

	/* ArcResourceLink() = default; */
	/* ~ArcResourceLink() = default; */

	ARCHandle *GetArcHandle() { return &mArcHandle; }
	char const *GetResRootDir() const { return mResRootDir; }

	void *GetArchiveDataStart() const;

	bool Set(void *archiveStart, char const *resourceRootDirectory);

	static u32 const RES_ROOT_DIR_SIZE = 128;

private:
	ut::LinkListNode mLink;					// _00
	ARCHandle mArcHandle;					// _08
	char mResRootDir[RES_ROOT_DIR_SIZE];	// _24
};

class ArcResourceAccessor : public ResourceAccessor {
public:
	ArcResourceAccessor();
	/* virtual ~ArcResourceAccessor() = default; */

	virtual void *GetResource(u32 resType, char const *name,
		                          u32 *pSize);
	virtual ut::Font *GetFont(char const *name);

	bool Attach(void *archiveStart, char const *resourceRootDirectory);
	void *Detach();

	void RegistFont(FontRefLink *pLink);
	void UnregistFont(FontRefLink *pLink);

	static u32 const RES_ROOT_DIR_SIZE = 128;

private:
	// _00     = VTBL
	// _00-_04 = ResourceAccessor
	ARCHandle mArcHandle;					// _04
	void *mArcBuf;							// _20
	FontRefLink::LinkList mFontList;		// _24
	char mResRootDir[RES_ROOT_DIR_SIZE];	// _30
};

class MultiArcResourceAccessor : public ResourceAccessor {
public:
	MultiArcResourceAccessor();
		
	virtual ~MultiArcResourceAccessor();
	virtual void *GetResource(u32 resType, char const *name,
		                          u32 *pSize);
	virtual ut::Font *GetFont(char const *name);

	void Attach(ArcResourceLink *pLink);
	void Detach(ArcResourceLink *pLink);
	void DetachAll();

	ArcResourceLink *Detach(void const *archiveStart);

	void RegistFont(FontRefLink *pLink);
	void UnregistFont(FontRefLink *pLink);

private:
	// _00     = VTBL
	// _00-_04 = ResourceAccessor
	ArcResourceLink::LinkList mArcList;	// _04
	FontRefLink::LinkList mFontList;	// _10
};

namespace detail {
	ut::Font *FindFont(FontRefLink::LinkList *pFontRefList,
		                   char const *name);
} // namespace detail
} // namespace lyt
} // namespace nw4hbm

#endif // NW4HBM_LYT_ARC_RESOURCE_ACCESSOR_H
