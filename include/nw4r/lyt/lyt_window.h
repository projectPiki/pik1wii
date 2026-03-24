#ifndef NW4R_LYT_WINDOW_H
#define NW4R_LYT_WINDOW_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_pane.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

// Forward declarations
class Material;
struct ResBlockSet;

/******************************************************************************
 *
 * InflationLRTB
 *
 ******************************************************************************/
struct InflationLRTB {
    f32 l; 	  // _00
    f32 r; 	  // _04
    f32 t; 	  // _08
    f32 b; 	  // _0C
};

namespace res {

/******************************************************************************
 *
 * WindowContent
 *
 ******************************************************************************/
struct WindowContent {
    u32 vtxCols[VERTEXCOLOR_MAX]; 	  // _00
    u16 materialIdx;              	  // _10
    u8 texCoordNum;               	  // _12
    u8 PADDING_0x13[0x14 - 0x13]; 	  // _13
};

/******************************************************************************
 *
 * WindowFrame
 *
 ******************************************************************************/
struct WindowFrame {
    u16 materialIdx;	// _00
    u8 textureFlip; 	// _02
    u8 PADDING_0x3; 	// _03
};

/******************************************************************************
 *
 * WND1 binary layout
 *
 ******************************************************************************/
struct Window : public Pane {
    static const u32 SIGNATURE = FOURCC('w', 'n', 'd', '1');

    InflationLRTB inflation;    	// _4C
    u8 frameNum;                	// _5C
    u8 PADDING_0x5D;            	// _5D
    u8 PADDING_0x5E;            	// _5E
    u8 PADDING_0x5F;            	// _5F
    u32 contentOffset;          	// _60
    u32 frameOffsetTableOffset; 	// _64
};

} // namespace res

/******************************************************************************
 *
 * WindowFrameEnum
 *
 ******************************************************************************/
enum WindowFrameEnum {
    WINDOWFRAME_LT,
    WINDOWFRAME_RT,
    WINDOWFRAME_LB,
    WINDOWFRAME_RB,

    WINDOWFRAME_L,
    WINDOWFRAME_R,
    WINDOWFRAME_T,
    WINDOWFRAME_B,

    WINDOWFRAME_MAX
};

/******************************************************************************
 *
 * WindowFrameSize
 *
 ******************************************************************************/
struct WindowFrameSize {
    f32 l;    // _00
    f32 r;    // _04
    f32 t;    // _08
    f32 b;    // _0C
};

/******************************************************************************
 *
 * Window
 *
 ******************************************************************************/
class Window : public Pane {
public:
    NW4R_UT_RTTI_DECL(Window);

public:
    Window(const res::Window* pRes, const ResBlockSet& rBlockSet);
    
	virtual ~Window(); 														     // _08
    virtual void DrawSelf(const DrawInfo& rInfo); 					  		     // _18
    virtual void AnimateSelf(u32 option);     					  		         // _20
    virtual ut::Color GetVtxColor(u32 idx) const;       	  		             // _24
    virtual void SetVtxColor(u32 idx, ut::Color color); 		          	     // _28
    virtual u8 GetVtxColorElement(u32 idx) const;       		         	     // _34
    virtual void SetVtxColorElement(u32 idx, u8 value); 		        	     // _38
    virtual Material* FindMaterialByName(const char* pName, bool recursive);     // _40
    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans); 	     		 // _50
    virtual u8 GetMaterialNum() const;                                     	     // _64
    virtual Material *GetMaterial(u32 idx) const;                             	 // _6C
    virtual Material* GetContentMaterial() const;      	   					     // _74
    virtual Material* GetFrameMaterial(u32 idx) const; 	    					 // _78
	
	
	void InitContent(u8 texNum);
    void InitFrame(u8 frameNum);
    void ReserveTexCoord(u8 num);

protected:
    struct Frame {
        u8 textureFlip;      // _00
        Material* pMaterial; // _04

        Frame() : textureFlip(0), pMaterial(nullptr) {}
    };

    struct Content {
        ut::Color vtxColors[VERTEXCOLOR_MAX];  	  // _00
        detail::TexCoordAry texCoordAry;       	  // _10
    };

protected:

	virtual void DrawContent(const math::VEC2& rBase,
                             const WindowFrameSize& rFrameSize,
                             u8 alpha); 	    								 // _7C

    virtual void DrawFrame(const math::VEC2& rBase, const Frame& rFrame,
                           const WindowFrameSize& rFrameSize,
                           u8 alpha); 	    									 // _80

    virtual void DrawFrame4(const math::VEC2& rBase, const Frame* pFrames,
                            const WindowFrameSize& rFrameSize,
                            u8 alpha); 	    									 // _84

    virtual void DrawFrame8(const math::VEC2& rBase, const Frame* pFrames,
                            const WindowFrameSize& rFrameSize,
                            u8 alpha); 	   									     // _88
    

    WindowFrameSize GetFrameSize(u8 frameNum, const Frame* pFrames);

protected:
    InflationLRTB mContentInflation; 	 // _D8
    Content mContent;                	 // _E8
    Frame* mFrames;                  	 // _100
    u8 mFrameNum;                    	 // _104
};

} // namespace lyt
} // namespace nw4r

#endif
