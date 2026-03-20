#ifndef NW4HBM_LYT_DRAW_INFO_H
#define NW4HBM_LYT_DRAW_INFO_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "homebuttonLib/nw4hbm/math/types.h"
#include "homebuttonLib/nw4hbm/ut/Rect.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace lyt
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:lyt_drawInfo.o(1)::.debug_info::0x125 [original object]
	class DrawInfo
	{
	// methods
	public:
		// cdtors
		DrawInfo();
		virtual ~DrawInfo();

		// methods
		math::MTX34 const &GetViewMtx() const { return mViewMtx; }
		math::VEC2 const &GetLocationAdjustScale() const
		{
			return mLocationAdjustScale;
		}

		f32 GetGlobalAlpha() const { return mGlobalAlpha; }
		bool IsMultipleViewMtxOnDraw() const { return mFlag.mulViewDraw; }
		bool IsInfluencedAlpha() const { return mFlag.influencedAlpha; }
		bool IsLocationAdjust() const { return mFlag.locationAdjust; }
		bool IsInvisiblePaneCalculateMtx() const
		{
			return mFlag.invisiblePaneCalculateMtx;
		}

		bool IsDebugDrawMode() const { return mFlag.debugDrawMode; }

		void SetViewMtx(math::MTX34 const &value) { mViewMtx = value; }
		void SetViewRect(ut::Rect const &value) { mViewRect = value; }
		void SetLocationAdjustScale(math::VEC2 const &scale)
		{
			mLocationAdjustScale = scale;
		}

		void SetGlobalAlpha(f32 alpha) { mGlobalAlpha = alpha; }
		void SetInfluencedAlpha(bool bEnable)
		{
			mFlag.influencedAlpha = bEnable;
		}

		void SetLocationAdjust(bool bEnable) { mFlag.locationAdjust = bEnable; }

		bool IsYAxisUp() const
		{
			return mViewRect.bottom - mViewRect.top < 0.0f;
		}

	// members
	public: // homebutton::HomeButton::draw
		math::MTX34	mViewMtx;				// size 0x30, offset 0x04
		ut::Rect	mViewRect;				// size 0x10, offset 0x34
		math::VEC2	mLocationAdjustScale;	// size 0x08, offset 0x44
		f32			mGlobalAlpha;			// size 0x04, offset 0x4c

		// [SC5PGN]/build/libs/Debug/slamWiiD.a:lyt_drawInfo.o(1)::.debug_info::0x548 [original object]
		struct /* explicitly untagged */
		{
			u8	mulViewDraw					: 1; // size 1, offset 0x00:0
			u8	influencedAlpha				: 1; // size 1, offset 0x00:1
			u8	locationAdjust				: 1; // size 1, offset 0x00:2
			u8	invisiblePaneCalculateMtx	: 1; // size 1, offset 0x00:3
			u8	debugDrawMode				: 1; // size 1, offset 0x00:4
			/* 3 bits padding */
		} mFlag; // size 0x01, offset 0x50
		/* 3 bytes padding */
	}; // size 0x54
}} // namespace nw4hbm::lyt

#endif // NW4HBM_LYT_DRAW_INFO_H
