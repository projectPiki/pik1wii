#ifndef _AICONSTANT_H
#define _AICONSTANT_H

#include "GlobalGameOptions.h"
#include "Node.h"
#include "Parameters.h"
#include "types.h"

/**
 * @brief A node that contains AI constants.
 *
 * @note Size: 0x194.
 */
struct AIConstant : public Node {

	/**
	 * @brief TODO
	 *
	 * @note Offset comments are relative to AIConstant for ease of use.
	 */
	struct Parms : public Parameters {
		inline Parms()
		    : Parameters("AI::Parms")
		    , mGravity(this, 550.0f, 0.0f, 1200.0f, "p00", MATCHING_PARM_NAME("じゅうりょく"))
		    , _34(this, 1, 0, 1, "p01", MATCHING_PARM_NAME("ピキ：オートアタック"))
		    , _44(this, 1, 0, 1, "p02", MATCHING_PARM_NAME("ピキ：オート抜き手伝い"))
		    , _54(this, 0, 0, 1, "p03", MATCHING_PARM_NAME("ナビ：抜きキャンセル"))
		    , _64(this, 1, 0, 1, "p04", MATCHING_PARM_NAME("毎フレーム押す"))
		    , mDoCStickAttack(this, 1, 0, 1, "p05", MATCHING_PARM_NAME("宮本接触バトル"))
		    , _84(this, 1, 0, 1, "p07", MATCHING_PARM_NAME("フリー時に敵から逃げる"))
		    , _94(this, 1, 0, 1, "p06", MATCHING_PARM_NAME("フリー時、ヘリポートに向かう"))
		    , _A4(this, 1, 0, 1, "p08", MATCHING_PARM_NAME("攻撃したら戻る"))
		    , _B4(this, 1, 0, 1, "p09", MATCHING_PARM_NAME("フォーメーションソート"))
		    , _C4(this, 1, 0, 1, "p10", MATCHING_PARM_NAME("１回だけ攻撃=1 死ぬまで攻撃=0"))
		    , mJumpTriangleAngleThreshold(this, 90.0f, 0.0f, 360.0f, "p11", MATCHING_PARM_NAME("オートジャンプ角度"))
		    , _E4(this, 1, 0, 1, "p12", MATCHING_PARM_NAME("投げ=1 転がし=0"))
		    , mDoPluckWithCursor(this, 1, 0, 1, "p13", MATCHING_PARM_NAME("カーソル抜き"))
		    , mDoScaleHappaMoveSpeed(this, 1, 0, 1, "p14", MATCHING_PARM_NAME("花ピキ足早"))
		    , mMaxPikisOnField(this, MAX_PIKI_ON_FIELD, 0, 120, "p15", MATCHING_PARM_NAME("ピキ＋芽 リミット"))
		    , _124(this, 1, 0, 1, "p16", MATCHING_PARM_NAME("爆弾方式"))
		    , mWeakSlipFactor(this, 1.0f, 0.0f, 10.0f, "p17", MATCHING_PARM_NAME("すべり弱"))
		    , mStrongSlipFactor(this, 2.5f, 0.0f, 10.0f, "p18", MATCHING_PARM_NAME("すべり強"))
		    , _154(this, 6, 2, 30, "p19", MATCHING_PARM_NAME("UFO LEVEL 2"))
		    , _164(this, 12, 2, 30, "p20", MATCHING_PARM_NAME("UFO LEVEL 3"))
		    , _174(this, 29, 2, 30, "p21", MATCHING_PARM_NAME("UFO LEVEL 4"))
		    , _184(this, 30, 2, 30, "p22", MATCHING_PARM_NAME("UFO LEVEL 5"))
		{
		}

		// _20-_24 = Parameters
		Parm<f32> mGravity;                    // _24, p00
		Parm<int> _34;                         // _34, p01
		Parm<int> _44;                         // _44, p02
		Parm<int> _54;                         // _54, p03
		Parm<int> _64;                         // _64, p04
		Parm<int> mDoCStickAttack;             // _74, p05, will csticking pikis into enemies cause the pikis to attack?
		Parm<int> _84;                         // _84, p07
		Parm<int> _94;                         // _94, p06
		Parm<int> _A4;                         // _A4, p08
		Parm<int> _B4;                         // _B4, p09
		Parm<int> _C4;                         // _C4, p10, meeo: single attack only toggle?
		Parm<f32> mJumpTriangleAngleThreshold; // _D4, p11
		Parm<int> _E4;                         // _E4, p12, meeo: throw toggle (vs rolling pikis lol)?
		Parm<int> mDoPluckWithCursor;          // _F4, p13, i.e. pluckaphone toggle
		Parm<int> mDoScaleHappaMoveSpeed;      // _104, p14, bud and flower pikis walk faster than leaves
		Parm<int> mMaxPikisOnField;            // _114, p15, THE field limit (100 by default)
		Parm<int> _124;                        // _124, p16, meeo: drop bomb type (immediately vs whistled)?
		Parm<f32> mWeakSlipFactor;             // _134, p17
		Parm<f32> mStrongSlipFactor;           // _144, p18
		Parm<int> _154;                        // _154, p19, meeo: UFO level 2 (forest navel)?
		Parm<int> _164;                        // _164, p20, meeo: UFO level 3 (distant spring)?
		Parm<int> _174;                        // _174, p21, meeo: UFO level 4 (final trial)?
		Parm<int> _184;                        // _184, p22, meeo: UFO level 5 (NOT perfect ending trigger)?
	};

	AIConstant();

	virtual void read(RandomAccessStream& input) // _0C (weak)
	{
		mConstants.read(input);
	}

	static void createInstance();

	static AIConstant* _instance;

	// _00     = VTBL
	// _00-_20 = Node
	Parms mConstants; // _20
};

#endif
