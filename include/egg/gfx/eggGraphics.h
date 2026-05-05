#ifndef EGG_GFX_GRAPHICS_H
#define EGG_GFX_GRAPHICS_H

#include <egg/egg_types.h>
#include <egg/math/eggBoundBox.h>
#include <egg/gfx/eggTexture.h>

namespace EGG {

class Graphics {
public:
  

    static void setupGX();
    static void enable_zbuffer();
    static void disable_zbuffer();
    void drawQuad2Lyt(EGG::BoundBox2f&, EGG::Texture*);

//protected:
  //  Matrix34f mViewMtx;
    //Matrix34f mOtherMtx;
};


} // namespace EGG

#endif