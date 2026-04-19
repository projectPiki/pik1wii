#ifndef _GAMECORESECTION_H
#define _GAMECORESECTION_H

#include "Light.h"
#include "Node.h"
#include "types.h"

struct Camera;
struct Controller;
struct Creature;
struct Font;
struct MapMgr;
struct Menu;
struct Navi;
struct RectArea;
struct SearchSystem;

namespace zen {
struct DrawGameInfo;
}

/**
 * @brief TODO
 */
enum CorePauseFlags {
	COREPAUSE_Unk1 = 1 << 0, // 0x1
	COREPAUSE_Unk2 = 1 << 1, // 0x2
	COREPAUSE_Unk3 = 1 << 2, // 0x4
	COREPAUSE_Unk4 = 1 << 3, // 0x8
	// ...
	COREPAUSE_Unk16 = 1 << 15, // 0x8000
};

BEGIN_ENUM_TYPE(GameHideFlags)
enum {
	ShowPellets             = 1 << 0, // 0x1
	ShowPelletsExceptSucked = 1 << 1, // 0x2
	ShowTeki                = 1 << 2, // 0x4 aka enemies
} END_ENUM_TYPE;

/**
 * @brief TODO
 */
struct GameCoreSection : public Node {
	GameCoreSection(Controller*, MapMgr*, Camera&);

	virtual void update();        // _10
	virtual void draw(Graphics&); // _14

	void updateTextDemo();
	void startMovie(u32, bool);
#if defined(VERSION_PIKIDEMO)
	void endMovie();
#else
	void endMovie(int movieIdx);
#endif
	void exitDayEnd();
	void forceDayEnd();
	void clearDeadlyPikmins();
	void enterFreePikmins();
	void cleanupDayEnd();
	void prepareBadEnd();
	void exitStage();
	void initStage();
	void finalSetup();
	void startContainerDemo();
	void startSundownWarn();
	void updateAI();
	void draw1D(Graphics&);
	void draw2D(Graphics&);

	static void startTextDemo(Creature*, int);

	// unused/inlined:
	bool hideTeki();
	bool hideAllPellet();
	bool hidePelletExceptSucked();

	static void finishPause() { pauseFlag = 0; }
	static void startPause(u16 pause) { pauseFlag = pause; }
	static bool inPause() { return pauseFlag & COREPAUSE_Unk16; } // probably?

	static u16 pauseFlag;
	static int textDemoState;
	static u16 textDemoTimer;
	static int textDemoIndex;

	// _00     = VTBL
	// _00-_20 = Node
	Controller* mController;          // _20
	int mDrawHideType;                // _24, enum todo
	u32 mHideFlags;                   // _28, see GameHideFlags enum
	bool mUseMovieBackCamera;         // _2C
	bool mDoneSundownWarn;            // _2D
	u32 _30;                          // _30, unknown
	bool mIsTimePastQuarter1;         // _34
	bool mIsTimePastNoon;             // _35
	bool mIsTimePastQuarter3;         // _36
	Menu* mAiPerfDebugMenu;           // _38, unknown
	u8 _3C[0x4C - 0x3C];              // _3C, unknown
	Shape* mPikiShape;                // _4C, unknown
	SearchSystem* mSearchSystem;      // _50
	Navi* mNavi;                      // _54
	u8 _58[0x60 - 0x58];              // _58, unknown
	MapMgr* mMapMgr;                  // _60
	Texture* mShadowTexture;          // _64
	Font* mBigFont;                   // _68
	Light _6C;                        // _6C
	zen::DrawGameInfo* mDrawGameInfo; // _340
};

#endif
