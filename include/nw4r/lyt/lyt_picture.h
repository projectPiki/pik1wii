#ifndef NW4R_LYT_PICTURE_H
#define NW4R_LYT_PICTURE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_pane.h>

namespace nw4r {
namespace lyt {

// Forward declarations
struct ResBlockSet;
class TexMap;

namespace res {

/******************************************************************************
 *
 * PIC1 binary layout
 *
 ******************************************************************************/
struct Picture : Pane {
    static const u32 SIGNATURE = FOURCC('p', 'i', 'c', '1');

    u32 vtxCols[VERTEXCOLOR_MAX]; // _4C
    u16 materialIdx;              // _5C
    u8 texCoordNum;               // _5E
    u8 PADDING_0x5F;              // _5F
};

} // namespace res

/******************************************************************************
 *
 * Picture
 *
 ******************************************************************************/
class Picture : public Pane {
public:
    NW4R_UT_RTTI_DECL(Picture);

public:
    Picture(u8 num);
    Picture(const res::Picture* pRes, const ResBlockSet& rBlockSet);
    
	virtual ~Picture(); 								// _08
    virtual void DrawSelf(const DrawInfo& rInfo); 		// _18
    virtual ut::Color GetVtxColor(u32 idx) const;       // _24
    virtual void SetVtxColor(u32 idx, ut::Color color); // _28
    virtual u8 GetVtxColorElement(u32 idx) const;       // _34
    virtual void SetVtxColorElement(u32 idx, u8 value); // _38
    virtual void Append(const TexMap& rTexMap); 		// _78

protected:
    ut::Color mVtxColors[VERTEXCOLOR_MAX]; // _D8
    detail::TexCoordAry mTexCoordAry;      // _E8

protected:
    void ReserveTexCoord(u8 num);
    void SetTexCoordNum(u8 num);

private:
    void Init(u8 num);
};

} // namespace lyt
} // namespace nw4r

#endif
