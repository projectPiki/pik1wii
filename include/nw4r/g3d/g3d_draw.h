#ifndef NW4R_G3D_DRAW_H
#define NW4R_G3D_DRAW_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resmat.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <nw4r/g3d/res/g3d_resshp.h>

#include <nw4r/math.h>

namespace nw4r {
namespace g3d {

// Forward declarations
namespace G3DState {
class IndMtxOp;
} // namespace G3DState

namespace detail {

G3DState::IndMtxOp* GetIndMtxOp(ResMat mat, ResNode node, ResShp shp);

} // namespace detail

struct DrawResMdlReplacement {
	u32 flag;                                   // _00
    u8* visArray;                               // _04
    ResTexObjData* texObjDataArray;             // _08
    ResTlutObjData* tlutObjDataArray;           // _0C
    ResTexSrtData* texSrtDataArray;             // _10
    ResChanData* chanDataArray;                 // _14
    ResGenModeData* genModeDataArray;           // _18
    ResMatMiscData* matMiscDataArray;           // _1C
    ResPixDL* pixDLArray;                       // _20
    ResTevColorDL* tevColorDLArray;             // _24
    ResIndMtxAndScaleDL* indMtxAndScaleDLArray; // _28
    ResTexCoordGenDL* texCoordGenDLArray;       // _2C
    ResTevData* tevDataArray;                   // _30
    ResVtxPosData** vtxPosTable;                // _34
    ResVtxNrmData** vtxNrmTable;                // _38
    ResVtxClrData** vtxClrTable;                // _3C
};

void DrawResMdlDirectly(const ResMdl mdl, const math::MTX34* pViewPosMtxArray,
                        const math::MTX33* pViewNrmMtxArray,
                        const math::MTX34* pViewEnvMtxArray,
                        const u8* pByteCodeOpa, const u8* pByteCodeXlu,
                        DrawResMdlReplacement* pReplacement, u32 drawMode);

} // namespace g3d
} // namespace nw4r

#endif
