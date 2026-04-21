#ifndef EGG_CORE_CONTROLLER_H
#define EGG_CORE_CONTROLLER_H

#include <egg/egg_types.h>

#include <egg/core/eggSingleton.h>
#include <egg/math/eggMatrix.h>
#include <egg/math/eggVector.h>
#include <egg/prim/eggBitFlag.h>
#include <egg/prim/eggBuffer.h>

#include <nw4r/ut.h>

#include <RevoSDK/kpad.h>
#include <RevoSDK/pad.h>
#include <RevoSDK/wpad.h>

namespace EGG {

// Forward declarations
class CoreController;
class ControllerRumbleMgr;

enum eCoreDevType {
    cDEV_CORE = WPAD_DEV_CORE,
    cDEV_FREESTYLE = WPAD_DEV_FREESTYLE,
    cDEV_CLASSIC = WPAD_DEV_CLASSIC,

    cDEV_FUTURE = WPAD_DEV_FUTURE,
    cDEV_NOT_SUPPORTED = WPAD_DEV_NOT_SUPPORTED,
    cDEV_NOT_FOUND = WPAD_DEV_NOT_FOUND,
    cDEV_NULL = WPAD_DEV_NULL,
    cDEV_UNKNOWN = WPAD_DEV_UNKNOWN,
};

enum eCoreButton {
    cCORE_BUTTON_UP = 1 << 3,
    cCORE_BUTTON_DOWN = 1 << 2,
    cCORE_BUTTON_LEFT = 1 << 0,
    cCORE_BUTTON_RIGHT = 1 << 1,

    cCORE_BUTTON_PLUS = 1 << 4,
    cCORE_BUTTON_MINUS = 1 << 12,

    cCORE_BUTTON_1 = 1 << 9,
    cCORE_BUTTON_2 = 1 << 8,

    cCORE_BUTTON_A = 1 << 11,
    cCORE_BUTTON_B = 1 << 10,

    cCORE_BUTTON_HOME = 1 << 15,

    cCORE_FSSTICK_UP = 1 << 16,
    cCORE_FSSTICK_DOWN = 1 << 17,
    cCORE_FSSTICK_LEFT = 1 << 18,
    cCORE_FSSTICK_RIGHT = 1 << 19,

    cCORE_FSSTICK_BUTTONS = cCORE_FSSTICK_UP | cCORE_FSSTICK_DOWN |
                            cCORE_FSSTICK_LEFT | cCORE_FSSTICK_RIGHT
};

typedef CoreController* (*CoreControllerFactory)();

struct CoreControllerConnectArg {
    s32 chan;   // _00
    s32 result; // _04
};

typedef void (*CoreControllerConnectCallback)(
    const CoreControllerConnectArg& rArg);

struct CoreControllerExtensionArg {
    eCoreDevType newDevType; // _00
    eCoreDevType oldDevType; // _04
    s32 chan;                // _08
};

class CoreControllerExtensionCallback {
public:
    virtual void onConnect(const CoreControllerExtensionArg& rArg); // _08
};

/******************************************************************************
 *
 * CoreStatus
 *
 ******************************************************************************/
class CoreStatus : public KPADStatus {
    friend class CoreController;

public:
    CoreStatus() {}

    u32 getHold() const {
        return hold;
    }
    u32 getRelease() const {
        return release;
    }
    u32 getTrigger() const {
        return trig;
    }

    bool down(u32 buttons) const {
        return hold & buttons;
    }
    bool up(u32 buttons) const {
        return (hold & buttons) != buttons;
    }
	
	bool downAll(u32 buttons) const {
        return (buttons & hold) == buttons;
    }
    bool upAll(u32 buttons) const {
        return (buttons & hold) == 0;
    }

    bool downTrigger(u32 buttons) const {
        return trig & buttons;
    }
    bool upTrigger(u32 buttons) const {
        return release & buttons;
    }

    Vector3f getAccel() const DECOMP_DONT_INLINE{
        return Vector3f(acc.x, acc.y, acc.z);
    }

    eCoreDevType getDevType() const {
        return static_cast<eCoreDevType>(dev_type);
    }

    bool isCore() const {
        return getDevType() == WPAD_DEV_CORE || isFreestyle();
    }
    bool isFreestyle() const {
        return getDevType() == WPAD_DEV_FREESTYLE;
    }

    s8 getDPDValidFlag() const {
        return dpd_valid_fg;
    }

    f32 getFSStickX() const {
        return ex_status.fs.stick.x;
    }
    f32 getFSStickY() const {
        return ex_status.fs.stick.y;
    }

    u32 getFSStickButton();
};

/******************************************************************************
 *
 * CoreController
 *
 ******************************************************************************/
class CoreController {
    friend class CoreControllerMgr;

public:
    CoreController();

    virtual void setPosParam(f32 playRadius, f32 sensitivity) 		// _08
	{
        KPADSetPosParam(mChannelID, playRadius, sensitivity);
    } 																
    virtual void setHoriParam(f32 playRadius, f32 sensitivity) 		// _0C
	{
        KPADSetHoriParam(mChannelID, playRadius, sensitivity);
    } 																
    virtual void setDistParam(f32 playRadius, f32 sensitivity) 		// _10
	{
        KPADSetDistParam(mChannelID, playRadius, sensitivity);
    } 																
    virtual void setAccParam(f32 playRadius, f32 sensitivity)		// _14
	{
        KPADSetAccParam(mChannelID, playRadius, sensitivity);
    } 
	virtual bool down(u32 buttons) const 							// _19
	{
        return mCoreStatus[0].down(buttons);
    }
    virtual bool up(u32 buttons) const 								// _1C
	{
        return mCoreStatus[0].up(buttons);
    }
    virtual bool downTrigger(u32 buttons) const 					// _20
	{
        return mCoreStatus[0].downTrigger(buttons);
    }
    virtual bool upTrigger(u32 buttons) const 						// _24
	{
        return mCoreStatus[0].upTrigger(buttons);
    }
    virtual bool downAll(u32 buttons) const 						// _28
	{
        return mCoreStatus[0].downAll(buttons);
    }
    virtual bool upAll(u32 buttons) const 							// _2C
	{
        return mCoreStatus[0].upAll(buttons);
	}
	virtual void beginFrame(PADStatus*); 							// _30
    virtual void endFrame();                            			// _34


    

    s32 getChannelID() const {
        return mChannelID;
    }

    Vector3f getAccel() const DECOMP_DONT_INLINE {
        return mCoreStatus[0].getAccel();
    }

    eCoreDevType getDevType() const {
        return mCoreStatus[0].getDevType();
    }

    bool isCore() const {
        return mCoreStatus[0].isCore();
    }
    bool isFreestyle() const {
        return mCoreStatus[0].isFreestyle();
    }

    s32 getReadLength() const {
        return mKPADReadLength;
    }

    bool isStable(u8 bits) const {
        return (mStableFlags & bits) == bits;
    }

    void setStableFrame(int frame) {
        mStableFrame = frame;
    }
    void setStableMag(f32 mag) {
        mStableMag = mag;
    }

    void sceneReset() DECOMP_DONT_INLINE;

    void startMotor();
    void stopMotor();

    void createRumbleMgr(u8 overlap_num);
    void startPatternRumble(const char* pPattern, int frame, bool force);
    void startPowerFrameRumble(f32 power, int frame, bool force);
    void stopRumbleMgr();
	
	Vector2f getDpdRawPos();
    f32 getDpdDistance();
    s32 getDpdValidFlag();
    f32 getFreeStickX();
    f32 getFreeStickY();

    CoreStatus* getCoreStatus(int index);
    CoreStatus* getCoreStatus() {
        return getCoreStatus(0);
    }

private:
    enum StableAxis {
        STABLE_AXIS_X,
        STABLE_AXIS_Y,
        STABLE_AXIS_Z,

        STABLE_AXIS_MAX
    };

    enum StableFlag {
        STABLE_FLAG_X = 1 << STABLE_AXIS_X,
        STABLE_FLAG_Y = 1 << STABLE_AXIS_Y,
        STABLE_FLAG_Z = 1 << STABLE_AXIS_Z,

        STABLE_FLAG_XYZ = STABLE_FLAG_X | STABLE_FLAG_Y | STABLE_FLAG_Z
    };

private:
    void calc_posture_matrix(Matrix34f& rPostureMtx, bool checkStable);

private:
    s32 mChannelID; 							// _04
    u32 mFSStickHold;    						// _08
    u32 mFSStickTrig;    						// _0C
    u32 mFSStickRelease; 						// _10
    CoreStatus mCoreStatus[KPAD_MAX_SAMPLES];   // _14
    s32 mKPADReadLength;                        // _F18
	TBitFlag<u8> mFlag;                         // _F1C
    Vector3f mAccelPrev;                        // _F20
	Vector2f mDpdPosPrev;                       // _F2C
    u32 mIdleTime;                       	 	// _F34
    u32 mSimpleRumblePattern;        			// _F38
    bool mEnableSimpleRumble;       			// _F3C
    int mSimpleRumbleFrame;          			// _F40
    u8 mSimpleRumbleSize;            			// _F44
    u8 mSimpleRumbleIndex;           			// _F45
    ControllerRumbleMgr* mRumbleMgr; 			// _F48
    Matrix34f mPostureMtx;     					// _F4C
    Matrix34f mPostureMtxPrev; 					// _F7C
    u8 mStableFlags;                   			// _FAC
    int mStableFrame;                  			// _FB0
    int mStableTimer[STABLE_AXIS_MAX]; 			// _FB4
    f32 mStableMag;                    			// _FC0
    Vector3f mStableAccel;             			// _FC4
};

/******************************************************************************
 *
 * CoreControllerMgr
 *
 ******************************************************************************/
class CoreControllerMgr {
    EGG_SINGLETON_DECL(CoreControllerMgr);

public:
    virtual void beginFrame(); // _08
    virtual void endFrame();   // _0C

    CoreController* getNthController(int index);

private:
    CoreControllerMgr();

    static void connectCallback(s32 chan, s32 result);

private:
    TBuffer<CoreController*> mControllers;               // _14
    CoreControllerExtensionCallback* mExtensionCallback; // _20
    TBuffer<eCoreDevType> mDevTypes;                     // _24

    static CoreControllerFactory sCoreControllerFactory;
    static CoreControllerConnectCallback sConnectCallback;
	static bool sWPADRegisterAllocatorOFF;
    static s32 sWPADWorkSize;
};

/******************************************************************************
 *
 * ControllerRumbleUnit
 *
 ******************************************************************************/
class ControllerRumbleUnit {
    friend class ControllerRumbleMgr;

public:
    ControllerRumbleUnit() {
        init();
    }

    virtual ~ControllerRumbleUnit() {} // _08

    void startPattern(const char* pPattern, int frame);
    void startPowerFrame(f32 power, int frame);

    f32 calc();

private:
    enum {
        BIT_USE_PATTERN,
        BIT_USE_POWER,

        BIT_POWER_LOOP,
        BIT_ENABLED,
        BIT_PATTERN_LOOP,
        BIT_PATTERN_TIMER,
    };

private:
    void init();

    void setFlags(u8 bit) {
        mFlags.setBit(bit);
    }
    void clrFlags(u8 bit) {
        mFlags.resetBit(bit);
    }
    bool tstFlags(u8 bit) {
        return mFlags.onBit(bit);
    }

private:
    const char* mRumblePattern;    // _04
    const char* mRumblePatternPtr; // _08
    int mRumbleFrame;              // _0C
    f32 mRumblePower;              // _10
    f32 mRumbleValue;              // _14
    TBitFlag<u8> mFlags;           // _18
    NW4R_UT_LIST_LINK_DECL();      // _1C
    char UNK_0x24[0x28 - 0x24];
};

/******************************************************************************
 *
 * ControllerRumbleMgr
 *
 ******************************************************************************/
class ControllerRumbleMgr {
public:
    ControllerRumbleMgr();
    virtual ~ControllerRumbleMgr() {} // _08

    void createUnit(u8 overlap_num, CoreController* pController);

    void stop();
    void calc();

    void startPattern(const char* pPattern, int frame, bool force);
    void startPowerFrame(f32 power, int frame, bool force);

private:
    ControllerRumbleUnit* getUnitFromList(bool force);

private:
    nw4r::ut::List mActiveUnitList;  // _00
    nw4r::ut::List mStoppedUnitList; // _10
    CoreController* mController;     // _14
};

} // namespace EGG

#endif
