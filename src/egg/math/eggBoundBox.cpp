#include <egg/math/eggBoundBox.h>
#include <egg/math/eggMath.h>

namespace EGG {

BoundBox2f::BoundBox2f() {
    f32 max = Math<f32>::maxNumber();
    mMin.set(max, max);
    mMax.set(-max, -max);
}

void BoundBox2f::resetBound() {
    f32 max = Math<f32>::maxNumber();
    mMin.set(max, max);
    mMax.set(-max, -max);
}

bool BoundBox2f::inside(const Vector2f &v) const {
    if (mMin.x <= v.x && v.x <= mMax.x && mMin.y <= v.y && v.y <= mMax.y) {
        return true;
    }

    return false;
}

} // namespace EGG