#ifndef NW4R_LYT_TYPES_H
#define NW4R_LYT_TYPES_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math.h>
#include <nw4r/ut.h>

#define NW4R_RES_NAME_SIZE 16
#define NW4R_MAT_NAME_SIZE 20
#define NW4R_LYT_PANE_USERDATA_LEN 8
#define NW4R_LYT_MATERIAL_NAME_LEN 20

#define TEXCOORD_VTX_COUNT 4

namespace nw4r {
namespace lyt {

// Forward declarations
class AnimTransform;

namespace detail {

/******************************************************************************
 *
 * Pointer operations
 *
 ******************************************************************************/
template <typename T> T* ConvertOffsToPtr(void* pBase, unsigned int offset) {
    return reinterpret_cast<T*>(reinterpret_cast<u8*>(pBase) + offset);
}
template <typename T> const T* ConvertOffsToPtr(const void* pBase, unsigned int offset) {
    return reinterpret_cast<const T*>(reinterpret_cast<const u8*>(pBase) +
                                      offset);
}

/******************************************************************************
 *
 * Bit operations
 *
 ******************************************************************************/
template <typename T> inline void SetBit(T* pBits, int pos, bool value) {
    T mask = ~(1 << pos);
    *pBits &= mask;
    *pBits |= (value ? 1 : 0) << pos;
}
template <typename T> inline bool TestBit(T bits, int pos) {
    T mask = 1 << pos;
    return (bits & mask) != 0;
}
template <typename T> inline T GetBits(T bits, int pos, int len) {
    T mask = ~(static_cast<T>(-1) << len);
    return bits >> pos & mask;
}

} // namespace detail

/******************************************************************************
 *
 * Size
 *
 ******************************************************************************/
struct Size {
    f32 width;       // _00
    f32 height;      // _04

    Size() : width(0.0f), height(0.0f) {}
    Size(const Size& rOther) : width(rOther.width), height(rOther.height) {}
    Size(f32 w, f32 h) : width(w), height(h) {}

    friend bool operator==(const Size& rLhs, const Size& rRhs) {
        return rLhs.width == rRhs.width && rLhs.height == rRhs.height;
    }
};

/******************************************************************************
 *
 * AnimationLink
 *
 ******************************************************************************/
class AnimationLink {
public:
    AnimationLink() : mbDisable(false) {
        Reset();
    }

    AnimTransform *GetAnimTransform() const {
        return mAnimTrans;
    }
    u16 GetIndex() const {
        return mIdx;
    }
    void SetIndex(u16 val) {
        mIdx = val;
    }

    bool IsEnable() const {
        return !mbDisable;
    }
    void SetEnable(bool enable) {
        mbDisable = !enable;
    }

    void Reset() {
        Set(nullptr, 0, false);
    }

    void Set(AnimTransform *pTransform, u16 us, bool b) {
        mAnimTrans = pTransform;
        mIdx = us;
        mbDisable = b;
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL();   // _00

private:
    AnimTransform* mAnimTrans;      // _08
    u16 mIdx;                       // _0C
    bool mbDisable;                 // _0E
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(AnimationLink);

/******************************************************************************
 *
 * AnimationGroupRef
 *
 ******************************************************************************/
struct AnimationGroupRef {
    const char *GetName() const {
        return name;
    }

    char name[17];      // _00
    u8 flag;            // _11
    u8 padding[2];      // _12
};

struct AnimationShareInfo {
    const char *GetTargetGroupName() const {
        return targetGroupName;
    }
    const char *GetSrcPaneName() const {
        return srcPaneName;
    }

    char srcPaneName[17];          // _00
    char targetGroupName[17];      // _11
    u8 padding[2];                 // _12
};

/******************************************************************************
 *
 * AlphaCompare
 *
 ******************************************************************************/
struct AlphaCompare {
    u8 comp; // _00
    u8 op;   // _01
    u8 ref0; // _02
    u8 ref1; // _03

    AlphaCompare() {
        Set(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }

    void Set(GXCompare comp0, u8 _ref0, GXAlphaOp _op, GXCompare comp1,
             u8 _ref1) {

        comp = (comp0 & 0b111) | ((comp1 & 0b111) << 4);
        op = _op;
        ref0 = _ref0;
        ref1 = _ref1;
    }

    GXCompare GetComp0() const {
        return static_cast<GXCompare>(comp & 0b1111);
    }
    GXCompare GetComp1() const {
        return static_cast<GXCompare>(comp >> 4 & 0b1111);
    }

    GXAlphaOp GetOp() const {
        return static_cast<GXAlphaOp>(op);
    }

    u8 GetRef0() const {
        return ref0;
    }
    u8 GetRef1() const {
        return ref1;
    }
};

/******************************************************************************
 *
 * BlendMode
 *
 ******************************************************************************/
struct BlendMode {
    u8 type;      // _00
    u8 srcFactor; // _01
    u8 dstFactor; // _02
    u8 op;        // _03

    BlendMode() {
        Set(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }

    void Set(GXBlendMode _type, GXBlendFactor src, GXBlendFactor dst,
             GXLogicOp _op) {

        type = _type;
        srcFactor = src;
        dstFactor = dst;
        op = _op;
    }

    GXBlendMode GetType() const {
        return static_cast<GXBlendMode>(type);
    }

    GXBlendFactor GetSrcFactor() const {
        return static_cast<GXBlendFactor>(srcFactor);
    }
    GXBlendFactor GetDstFactor() const {
        return static_cast<GXBlendFactor>(dstFactor);
    }

    GXLogicOp GetOp() const {
        return static_cast<GXLogicOp>(op);
    }
};

/******************************************************************************
 *
 * ChanCtrl
 *
 ******************************************************************************/
struct ChanCtrl {
    u8 matSrcCol; // _00
    u8 matSrcAlp; // _01
    u8 reserve1;  // _02
    u8 reserve2;  // _03

    ChanCtrl() : reserve1(0), reserve2(0) {
        Set(GX_SRC_VTX, GX_SRC_VTX);
    }

    void Set(GXColorSrc color, GXColorSrc alpha) {
        matSrcCol = color;
        matSrcAlp = alpha;
    }

    GXColorSrc GetColorSrc() const {
        return static_cast<GXColorSrc>(matSrcCol);
    }
    GXColorSrc GetAlphaSrc() const {
        return static_cast<GXColorSrc>(matSrcAlp);
    }
};

/******************************************************************************
 *
 * IndirectStage
 *
 ******************************************************************************/
struct IndirectStage {
    u8 texCoordGen; // _00
    u8 texMap;      // _01
    u8 scaleS;      // _02
    u8 scaleT;      // _03

    IndirectStage() {
        Set(GX_TEXCOORD0, GX_TEXMAP0, GX_ITS_1, GX_ITS_1);
    }

    void Set(GXTexCoordID gen, GXTexMapID map, GXIndTexScale _scaleS,
             GXIndTexScale _scaleT) {

        texCoordGen = gen;
        texMap = map;
        scaleS = _scaleS;
        scaleT = _scaleT;
    }

    GXTexCoordID GetTexCoordGen() const {
        return static_cast<GXTexCoordID>(texCoordGen);
    }

    GXTexMapID GetTexMap() const {
        return static_cast<GXTexMapID>(texMap);
    }

    GXIndTexScale GetScaleS() const {
        return static_cast<GXIndTexScale>(scaleS);
    }
    GXIndTexScale GetScaleT() const {
        return static_cast<GXIndTexScale>(scaleT);
    }
};

/******************************************************************************
 *
 * TevStageInOp
 *
 ******************************************************************************/
struct TevStageInOp {
    u8 ab; // _00
    u8 cd; // _01
    u8 op; // _02
    u8 cl; // _03

    u8 GetA() const {
        return ab & 0b1111;
    }
    u8 GetB() const {
        return ab >> 4 & 0b1111;
    }

    u8 GetC() const {
        return cd & 0b1111;
    }
    u8 GetD() const {
        return cd >> 4 & 0b1111;
    }

    void SetIn(u8 a, u8 b, u8 c, u8 d) {
        ab = a & 0b1111 | (b & 0b1111) << 4;
        cd = c & 0b1111 | (d & 0b1111) << 4;
    }

    u8 GetOp() const {
        return op & 0b1111;
    }
    u8 GetBias() const {
        return op >> 4 & 0b11;
    }
    u8 GetScale() const {
        return op >> 6 & 0b11;
    }

    bool IsClamp() const {
        return cl & 0b1;
    }
    u8 GetOutReg() const {
        return cl >> 1 & 0b11;
    }
    u8 GetKSel() const {
        return cl >> 3 & 0b11111;
    }

    void SetOp(u8 _op, u8 bias, u8 scale, bool clamp, u8 outReg, u8 ksel) {
        op = _op & 0b1111 | (bias & 0b11) << 4 | (scale & 0b11) << 6;
        cl = clamp & 0b1 | (outReg & 0b11) << 1 | (ksel & 0b11111) << 3;
    }
};

/******************************************************************************
 *
 * TevStage
 *
 ******************************************************************************/
struct TevStage {
    u8 texCoordGen;     // _00
    u8 colChan;         // _01
    u8 texMap;          // _02
    u8 swapSel;         // _03
    TevStageInOp colIn; // _04
    TevStageInOp alpIn; // _08
    u8 indStage;        // _0C
    u8 indBiMt;         // _0D
    u8 indWrap;         // _0E
    u8 indFoAdUtAl;     // _0F

    TevStage() {
        SetOrder(GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0, GX_TEV_SWAP0,
                 GX_TEV_SWAP0);

        SetColorIn(GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
        SetAlphaIn(GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);

        SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV,
                   GX_TEV_KCSEL_K0);
        SetAlphaOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV,
                   GX_TEV_KASEL_K0_R);

        SetIndirect(GX_IND_TEX_STAGE_0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF,
                    GX_ITW_OFF, GX_ITW_OFF, false, false, GX_ITBA_OFF);
    }

    void SetOrder(GXTexCoordID gen, GXTexMapID map, GXChannelID chan,
                  GXTevSwapSel ras, GXTevSwapSel tex) {

        texCoordGen = gen;
        colChan = chan;
        texMap = map;
        swapSel = (ras & 0b111) << 1 | (tex & 0b111) << 3 | (map >> 8);
    }

    void SetColorIn(GXTevColorArg a, GXTevColorArg b, GXTevColorArg c,
                    GXTevColorArg d) {

        colIn.SetIn(a, b, c, d);
    }
    void SetColorOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp,
                    GXTevRegID outReg, GXTevKColorSel ksel) {

        colIn.SetOp(op, bias, scale, clamp, outReg, ksel);
    }

    void SetAlphaIn(GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c,
                    GXTevAlphaArg d) {

        alpIn.SetIn(a, b, c, d);
    }
    void SetAlphaOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp,
                    GXTevRegID outReg, GXTevKAlphaSel ksel) {

        alpIn.SetOp(op, bias, scale, clamp, outReg, ksel);
    }

    void SetIndirect(GXIndTexStageID stage, GXIndTexFormat format,
                     GXIndTexBiasSel bias, GXIndTexMtxID mtx,
                     GXIndTexWrap wrapS, GXIndTexWrap wrapT, bool addPrev,
                     bool utcLod, GXIndTexAlphaSel alphaSel) {

        indStage = stage;
        indBiMt = bias & 0b111 | (mtx & 0b1111) << 4;
        indWrap = wrapS & 0b111 | (wrapT & 0b111) << 3;

        indFoAdUtAl = format & 0b111 | (addPrev ? 1 : 0) << 2 | utcLod << 3 |
                      (alphaSel & 0b11) << 4;
    }

    GXTexCoordID GetTexCoordGen() const {
        return static_cast<GXTexCoordID>(texCoordGen);
    }

    GXChannelID GetColorChan() const {
        return static_cast<GXChannelID>(colChan);
    }

    GXTexMapID GetTexMap() const {
        return static_cast<GXTexMapID>(texMap | (swapSel & 0b1) << 8);
    }

    GXTevSwapSel GetRasSwapSel() const {
        return static_cast<GXTevSwapSel>(swapSel >> 1 & 0b11);
    }
    GXTevSwapSel GetTexSwapSel() const {
        return static_cast<GXTevSwapSel>(swapSel >> 3 & 0b11);
    }

    GXTevColorArg GetColorInA() const {
        return static_cast<GXTevColorArg>(colIn.GetA());
    }
    GXTevColorArg GetColorInB() const {
        return static_cast<GXTevColorArg>(colIn.GetB());
    }
    GXTevColorArg GetColorInC() const {
        return static_cast<GXTevColorArg>(colIn.GetC());
    }
    GXTevColorArg GetColorInD() const {
        return static_cast<GXTevColorArg>(colIn.GetD());
    }

    GXTevOp GetColorOp() const {
        return static_cast<GXTevOp>(colIn.GetOp());
    }
    GXTevBias GetColorBias() const {
        return static_cast<GXTevBias>(colIn.GetBias());
    }
    GXTevScale GetColorScale() const {
        return static_cast<GXTevScale>(colIn.GetScale());
    }

    bool IsColorClamp() const {
        return colIn.IsClamp();
    }
    GXTevRegID GetColorOutReg() const {
        return static_cast<GXTevRegID>(colIn.GetOutReg());
    }
    GXTevKColorSel GetKColorSel() const {
        return static_cast<GXTevKColorSel>(colIn.GetKSel());
    }

    GXTevAlphaArg GetAlphaInA() const {
        return static_cast<GXTevAlphaArg>(alpIn.GetA());
    }
    GXTevAlphaArg GetAlphaInB() const {
        return static_cast<GXTevAlphaArg>(alpIn.GetB());
    }
    GXTevAlphaArg GetAlphaInC() const {
        return static_cast<GXTevAlphaArg>(alpIn.GetC());
    }
    GXTevAlphaArg GetAlphaInD() const {
        return static_cast<GXTevAlphaArg>(alpIn.GetD());
    }

    GXTevOp GetAlphaOp() const {
        return static_cast<GXTevOp>(alpIn.GetOp());
    }
    GXTevBias GetAlphaBias() const {
        return static_cast<GXTevBias>(alpIn.GetBias());
    }
    GXTevScale GetAlphaScale() const {
        return static_cast<GXTevScale>(alpIn.GetScale());
    }

    bool IsAlphaClamp() const {
        return alpIn.IsClamp();
    }
    GXTevRegID GetAlphaOutReg() const {
        return static_cast<GXTevRegID>(alpIn.GetOutReg());
    }
    GXTevKAlphaSel GetKAlphaSel() const {
        return static_cast<GXTevKAlphaSel>(alpIn.GetKSel());
    }

    GXIndTexStageID GetIndStage() const {
        return static_cast<GXIndTexStageID>(indStage);
    }

    GXIndTexBiasSel GetIndBiasSel() const {
        return static_cast<GXIndTexBiasSel>(indBiMt & 0b111);
    }
    GXIndTexMtxID GetIndMtxSel() const {
        return static_cast<GXIndTexMtxID>(indBiMt >> 3 & 0b1111);
    }

    GXIndTexWrap GetIndWrapS() const {
        return static_cast<GXIndTexWrap>(indWrap & 0b111);
    }
    GXIndTexWrap GetIndWrapT() const {
        return static_cast<GXIndTexWrap>(indWrap >> 3 & 0b111);
    }

    GXIndTexFormat GetIndFormat() const {
        return static_cast<GXIndTexFormat>(indFoAdUtAl & 0b11);
    }
    bool IsIndAddPrev() const {
        return indFoAdUtAl >> 2 & 0b1;
    }
    bool IsIndUtcLod() const {
        return indFoAdUtAl >> 3 & 0b1;
    }
    GXIndTexAlphaSel GetIndAlphaSel() const {
        return static_cast<GXIndTexAlphaSel>(indFoAdUtAl >> 4 & 0b11);
    }
};

/******************************************************************************
 *
 * TevSwapMode
 *
 ******************************************************************************/
struct TevSwapMode {
    u8 swap; // _00

    void Set(GXTevColorChan r, GXTevColorChan g, GXTevColorChan b,
             GXTevColorChan a) {

        swap = r | g << 2 | b << 4 | a << 6;
    }

    GXTevColorChan GetR() const {
        return static_cast<GXTevColorChan>(swap & 0b11);
    }
    GXTevColorChan GetG() const {
        return static_cast<GXTevColorChan>(swap >> 2 & 0b11);
    }
    GXTevColorChan GetB() const {
        return static_cast<GXTevColorChan>(swap >> 4 & 0b11);
    }
    GXTevColorChan GetA() const {
        return static_cast<GXTevColorChan>(swap >> 6 & 0b11);
    }
};

/******************************************************************************
 *
 * TexCoordGen
 *
 ******************************************************************************/
struct TexCoordGen {
    TexCoordGen() : reserve(0) {
        Set(GX_TG_MTX3X4, GX_TG_TEX0, GX_IDENTITY);
    }

    void Set(GXTexGenType type, GXTexGenSrc src, u32 mtxID) {
        texGenType = type;
        texGenSrc = src;
        texMtx = mtxID;
    }

    GXTexGenType GetTexGenType() const {
        return static_cast<GXTexGenType>(texGenType);
    }

    GXTexGenSrc GetTexGenSrc() const {
        return static_cast<GXTexGenSrc>(texGenSrc);
    }

    u32 GetTexMtx() const {
        return texMtx;
    }

    u8 texGenType; // _00
    u8 texGenSrc;  // _01
    u8 texMtx;     // _02
    u8 reserve;    // _03
};

/******************************************************************************
 *
 * TexSRT
 *
 ******************************************************************************/
struct TexSRT {
    math::VEC2 translate; // _00
    f32 rotate;           // _08
    math::VEC2 scale;     // _0C
};

} // namespace lyt
} // namespace nw4r

#endif
