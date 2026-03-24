#ifndef NW4R_G3D_SCN_MDL_H
#define NW4R_G3D_SCN_MDL_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_draw.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/res/g3d_resmat.h>

namespace nw4r {
namespace g3d {

// Forward declarations
class AnmObjShp;

class ScnMdl : public ScnMdlSimple {
public:
    class CopiedMatAccess {
    public:
        CopiedMatAccess(ScnMdl* pScnMdl, u32 id);

        ResTexObj GetResTexObj(bool markDirty);
        ResTexSrt GetResTexSrt(bool markDirty);
        ResMatChan GetResMatChan(bool markDirty);
        ResGenMode GetResGenMode(bool markDirty);
        ResMatPix GetResMatPix(bool markDirty);
        ResMatTevColor GetResMatTevColor(bool markDirty);
        ResTev GetResTev(bool markDirty);

        ResTexSrt GetResTexSrtEx();

    private:
        ScnMdl* mpScnMdl;                     // _00
        u32 mMatID;                           // _04
        ResTexObj mTexObj;                    // _08
        ResTlutObj mTlutObj;                  // _0C
        ResTexSrt mTexSrt;                    // _10
        ResMatChan mChan;                     // _14
        ResGenMode mGenMode;                  // _18
        ResMatMisc mMatMisc;                  // _1C
        ResMatPix mPix;                       // _20
        ResMatTevColor mTevColor;             // _24
        ResMatIndMtxAndScale mIndMtxAndScale; // _28
        ResMatTexCoordGen mTexCoordGen;       // _2C
        ResTev mTev;                          // _30
    };

    class CopiedVisAccess {
    public:
        CopiedVisAccess(ScnMdl* pScnMdl, u32 id);

        bool IsVisible() const;
        bool SetVisibilityEx(bool visible);

    private:
        ScnMdl* mpScnMdl; // _00
        u32 mNodeID;      // _04
        u8* mpVis;        // _08
    };

#define OPT(KEY, VALUE) OPTID_##KEY = (0x30000 | (VALUE))
    enum ScnMdlOption {
        OPT(NONE, 0),
        OPT(VISBUFFER_REFRESH_NEEDED, 1),
    };
#undef OPT

public:
    static ScnMdl* Construct(MEMAllocator* pAllocator, u32* pSize, ResMdl mdl,
                             u32 bufferOption, int numView);

    ScnMdl(MEMAllocator* pAllocator, ResMdl mdl, math::MTX34* pWorldMtxArray,
           u32* pWorldMtxAttribArray, math::MTX34* pViewPosMtxArray,
           math::MTX33* pViewNrmMtxArray, math::MTX34* pViewTexMtxArray,
           int numView, int numViewMtx, DrawResMdlReplacement* pReplacement,
           u32* pMatBufferDirtyFlag, u32 replacementFlag);

    virtual void G3dProc(u32 task, u32 param, void* pInfo); 	 	 // _0C
    virtual ~ScnMdl();                                      	 	 // _10
    virtual bool SetScnObjOption(u32 option, u32 value);         	 // _20
    virtual bool GetScnObjOption(u32 option, u32* pValue) const; 	 // _24
    virtual bool SetAnmObj(AnmObj* pObj, AnmObjType type);  	     // _34
    virtual bool RemoveAnmObj(AnmObj* pObj);                		 // _38
    virtual AnmObj* RemoveAnmObj(AnmObjType type);          		 // _3C
    virtual AnmObj* GetAnmObj(AnmObjType type);             		 // _40
    virtual const AnmObj* GetAnmObj(AnmObjType type) const; 		 // _44

    void InitBuffer();
    void CleanMatBuffer(u32 idx, u32 option);
    void CleanVisBuffer();

    AnmObjShp* GetAnmObjShp() {
        return mpAnmObjShp;
    }

    DrawResMdlReplacement& GetDrawResMdlReplacement() {
        return mReplacement;
    }

protected:
    void ScnMdl_G3DPROC_CALC_WORLD(u32 param, const math::MTX34* pParent);
    void ScnMdl_G3DPROC_CALC_MAT(u32 param, void* pInfo);
    void ScnMdl_G3DPROC_CALC_VTX(u32 param, void* pInfo);
    void ScnMdl_G3DPROC_DRAW_OPA(u32 param, void* pInfo);
    void ScnMdl_G3DPROC_DRAW_XLU(u32 param, void* pInfo);

private:
    enum VisBufferFlag {
        VISBUFFER_DIRTY = (1 << 0),
        VISBUFFER_NOT_REFRESH_NEEDED = (1 << 1),
    };
	
	enum MatBufferOption {
        BUFFER_RESTEXOBJ = (1 << 0),
        BUFFER_RESTLUTOBJ = (1 << 1),
        BUFFER_RESTEXSRT = (1 << 2),
        BUFFER_RESCHAN = (1 << 3),
        BUFFER_RESGENMODE = (1 << 4),
        BUFFER_RESMATMISC = (1 << 5),
        BUFFER_RESANMVIS = (1 << 6),
        BUFFER_RESMATPIX = (1 << 7),
        BUFFER_RESTEVCOLOR = (1 << 8),
        BUFFER_RESMATINDMTXSCALE = (1 << 9),
        BUFFER_RESMATTEXCOORDGEN = (1 << 10),
        BUFFER_RESTEV = (1 << 11),
        BUFFER_RESVTXPOS = (1 << 12),
        BUFFER_RESVTXNRM = (1 << 13),
        BUFFER_RESVTXCLR = (1 << 14),

        ANM_VIS = BUFFER_RESANMVIS,
        ANM_TEXPAT = BUFFER_RESTEXOBJ | BUFFER_RESTLUTOBJ,
        ANM_TEXSRT = BUFFER_RESTEXSRT | BUFFER_RESMATINDMTXSCALE,
        ANM_MATCLR = BUFFER_RESCHAN | BUFFER_RESTEVCOLOR,
        ANM_SHP = BUFFER_RESVTXPOS | BUFFER_RESVTXNRM | BUFFER_RESVTXCLR,

        BUFOPTION_0x1000000 = (1 << 24),
    };


private:
    bool IsVisBufferDirty() const {
        return mFlagVisBuffer & VISBUFFER_DIRTY;
    }
    bool IsVisBufferRefreshNeeded() const {
        return !(mFlagVisBuffer & VISBUFFER_NOT_REFRESH_NEEDED);
    }
    void VisBufferDirty() {
        mFlagVisBuffer |= VISBUFFER_DIRTY;
    }

    bool IsMatBufferDirty(u32 idx, u32 option) const {
        return option & mpMatBufferDirtyFlag[idx];
    }
    void MatBufferDirty(u32 idx, u32 option) {
        mpMatBufferDirtyFlag[idx] |= option;
    }

private:
    AnmObjShp* mpAnmObjShp;             // _138
    u32 mFlagVisBuffer;                 // _13C
    u32* mpMatBufferDirtyFlag;          // _140
    DrawResMdlReplacement mReplacement; // _144
	u32 mReplacementFlag;               // _184

    NW4R_G3D_RTTI_DECL_DERIVED(ScnMdl, ScnMdlSimple);
};

} // namespace g3d
} // namespace nw4r

#endif
