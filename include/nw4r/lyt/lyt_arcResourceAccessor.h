#ifndef NW4R_LYT_ARC_RESOURCE_ACCESSOR_H
#define NW4R_LYT_ARC_RESOURCE_ACCESSOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_resourceAccessor.h>

#include <nw4r/ut.h>

#include <RevoSDK/arc.h>

namespace nw4r {
namespace lyt {

/******************************************************************************
 *
 * FontRefLink
 *
 ******************************************************************************/
class FontRefLink {
public:
    const char* GetFontName() const {
        return mFontName;
    }

    ut::Font* GetFont() const {
        return mpFont;
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL(); // _00

protected:
    static const int FONTNAMEBUF_MAX = 128;

protected:
    char mFontName[FONTNAMEBUF_MAX]; // _08
    ut::Font* mpFont;                // _88
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(FontRefLink);

/******************************************************************************
 *
 * ArcResourceAccessor
 *
 ******************************************************************************/
class ArcResourceAccessor : public ResourceAccessor {
public:
    static const u32 RES_TYPE_NAME = 0;
    static const u32 RES_TYPE_ANIMATION = FOURCC('a', 'n', 'i', 'm');
    static const u32 RES_TYPE_LAYOUT = FOURCC('b', 'l', 'y', 't');
    static const u32 RES_TYPE_FONT = FOURCC('f', 'o', 'n', 't');
    static const u32 RES_TYPE_TEXTURE = FOURCC('t', 'i', 'm', 'g');

public:
    ArcResourceAccessor();

    virtual void* GetResource(u32 type, const char* pName,
                              u32* pSize); // _C

    virtual ut::Font* GetFont(const char* pName); // _10

    bool Attach(void* pArchive, const char* pRootDir);

    bool IsAttached() const {
        return mArcBuf != NULL;
    }

protected:
    static const int ROOTPATH_MAX = 128;

protected:
    ARCHandle mArcHandle;           // _04
    void* mArcBuf;                  // _20
    FontRefLinkList mFontList;      // _24
    char mResRootDir[ROOTPATH_MAX]; // _30
};

} // namespace lyt
} // namespace nw4r

#endif
