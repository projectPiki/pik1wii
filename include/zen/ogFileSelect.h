#ifndef _ZEN_OGFILESELECT_H
#define _ZEN_OGFILESELECT_H

#include "Colour.h"
#include "MemoryCard.h"
#include "P2D/TextBox.h"
#include "Vector.h"
#include "types.h"
#include "zen/SpectrumCursorMgr.h"
#include "zen/ogSub.h"
#include <string.h>

struct Controller;
struct CardQuickInfo;
struct Graphics;
struct P2DPicture;
struct P2DTextBox;

namespace zen {

struct EffectMgr2D;
struct ogNitakuMgr;
struct particleGenerator;

/**
 * @brief Converts special numeric codes within P2DTextBox strings. Used for dynamic text elements e.g. file numbers in prompts.
 *
 * @note Size: 0x108.
 */
struct ogCnvStringMgr {
public:
	ogCnvStringMgr(P2DTextBox* tbox)
	{
		mTextBox = tbox;
		mTextPtr = tbox->getString();
	}

	// DLL inline
	void convert()
	{
		strcpy(mTextBuffer, mTextPtr);
		cnvSpecialNumber(mTextBuffer);
		mTextBox->setString(mTextBuffer);
	}

private:
	P2DTextBox* mTextBox;    // _00
	char* mTextPtr;          // _04
	char mTextBuffer[0x100]; // _08
};

/**
 * @brief Manages the file selection screen, including display, user interaction,
 *		  and coordination of file operations (load, save, copy, delete).
 *
 * @note Size: 0x11A4.
 */
struct ogScrFileSelectMgr {
public:
	/**
	 * @brief Defines the current operation mode for file management.
	 */
	enum FileOperateMode {
		Normal = 0, // Standard file selection mode
		Copy   = 1, // File copying mode
		Delete = 2  // File deletion mode
	};

	/**
	 * @brief Flags indicating which title message to display.
	 */
	enum titleMessageFlag {
		SelectDataToLoadOrSave = 0, // Message for selecting a file to load or save
		SelectDataToSave,           // Message for selecting a file to save
		DataCorrupted,              // Message indicating data is corrupted
		NoMemoryCardInserted,       // Message indicating no memory card is inserted
		MemoryCardErrorOccurred,    // Message indicating a memory card error
		SelectSourceFileForCopy,    // Message for selecting the source file for copying
		ConfirmFileCopy,            // Message to confirm file copy, possibly with overwrite
		SelectFileForDelete,        // Message for selecting a file to delete
		ConfirmFileDelete,          // Message to confirm file deletion
		FormattingInProgress,       // Message indicating memory card formatting is in progress
		FormatComplete,             // Message indicating memory card formatting is complete
		SaveFailedError,            // Message indicating save operation failed
		SaveSuccessful,             // Message indicating save operation was successful
		MemoryCardFullError,        // Message indicating the memory card is full
	};

	/**
	 * @brief Defines the return status of the file select manager's update function, indicating the next action for the game flow.
	 *
	 * @todo These need fixing and de-AI-ing :< really? IPL?
	 */
	enum returnStatusFlag {
		Inactive = -1,  ///< -1, No action required, or initial state
		Continue,       ///< 0, Continue updating the file select screen
		FileChosen,     ///< 1, A file has been chosen (e.g., for loading/saving)
		ExitRequested,  ///< 2, User requested to exit the file select screen (e.g., cancel)
		OperationError, ///< 3, An error occurred during a file operation
		// Missing 4
		// Missing 5
		PostSaveAction = 6, ///< 6, Action after a save operation (e.g., refresh screen)
		SelectionA,         ///< 7, Action after a delete operation (e.g., refresh screen)
		SelectionB,         ///< 8, Action after a copy operation (e.g., refresh screen)
		SelectionC,         ///< 9, Memory card is busy, waiting for completion
		ReturnToIPL,        ///< 10, Request to return to the Initial Program Loader (IPL) or title screen
	};

	ogScrFileSelectMgr();

	returnStatusFlag update(Controller* pController, CardQuickInfo& outCardInfo);
	void start(bool isSaveMode, int fileSelectMode);
	void draw(Graphics& gfx);
	void quit();

private:
	void copyCardInfosSub();
	bool getCardFileInfos();
	void paneOnOffXY(bool isVisible);
	void MovePaneXY();
	void OpenYesNoWindow();
	void CloseYesNoWindow();
	void UpDateYesNoWindow();
	void setOperateMode_Normal();
	void setOperateMode(FileOperateMode mode);
	void getPane_FileTop1();
	void getPane_FileTop2();
	void setFileData(int fileSlotIndex);
	void set_SM_C();
	void getPane_FileIcon();
	void OnOffKetaNissuu(int fileSlotIndex);
	void getPane_CpyCurScreen();
	void SetTitleMsg(titleMessageFlag msgId);
	void ScaleAnimeTitle();
	void ScaleAnimeData();
	void setDataNumber(int fileSlotIndex);
	void init();
	void setDataScale();
	void chaseDataScale();
	void OnOffNewPane(int fileSlotIndex);
	void ChkOnyonOnOff();
	void ChkNewData();
	void BeginFadeOut();
	int CanToCopy(int sourceSlotIndex);
	void OperateSelect(Controller* pController);
	void KetteiEffectStart();
	void TailEffectStart();
	void TailEffectMove(int x, int y);
	void TailEffectMoveM(int x, int y);
	void setOperateMode_Delete();
	void setOperateMode_Copy();
	void OperateDelete(Controller* pController);
	void OperateCopy(Controller* pController);
	void DeleteEffectStart();
	void MoveCpyCursor(int targetSlotIndex, f32 animationSpeed);
	void CopyEffectStart();

	EffectMgr2D* mFxMgr;                                   // _00
	particleGenerator* mFileCopyEffectOnyon;               // _04
	particleGenerator* mFileCopyEffectPikminGroup;         // _08
	particleGenerator* mSelectionConfirmEffectOnyon;       // _0C
	particleGenerator* mSelectionConfirmEffectPikminGroup; // _10
	particleGenerator* mCursorMoveEffectOnyon;             // _14
	particleGenerator* mCursorMoveEffectPikminGroup;       // _18
	u8 _UNUSED1C[0x4];                                     // _1C, unknown
	returnStatusFlag mSelectState;                         // _20
	FileOperateMode mOperation;                            // _24
	bool mSaveMode;                                        // _28
	CardQuickInfo mCardInfo[90];                           // _2C
	SpectrumCursorMgr mCopyLeftCursor;                     // _E3C
	SpectrumCursorMgr mCopyRightCursor;                    // _E6C
	f32 mCopyCursorLPosX[3];                               // _E9C
	f32 mCopyCursorRPosX[3];                               // _EA8
	f32 mCopyCursorLPosY[3];                               // _EB4
	f32 mCopyCursorRPosY[3];                               // _EC0
	s16 mCurrSlotIdx;                                      // _ECC
	f32 _ED0[3];                                		   // _ED0
	f32 _EDC[3];                                		   // _EDC
	f32 mIconDataScales[3];                                // _EE8
	f32 mIconNoDataScales[3];                              // _EF4
	f32 mMainInteractTimer;                                // _F00
	int mMaxPartsNum;                                      // _F04
	int mOnyonIconPositionsX[3];                           // _F08
	int mOnyonIconPositionsY[3];                           // _F14
	int mNewIconPositionsX[3];                             // _F20
	int mNewIconPositionsY[3];                             // _F2C
	Vector3f mOnyonIconScales[3];                          // _F38
	Vector3f mNewIconScales[3];                            // _F5C
	int mOnyonIconCurrX[3];                                // _F80
	int mOnyonIconCurrY[3];                                // _F8C
	int mPikminIconCurrX[3];                               // _F98
	int mPikminIconCurrY[3];                               // _FA4
	int mNewIconInitX[3];                                  // _FB0
	int mNewIconInitY[3];                                  // _FBC
	int mNewIconCurrX[3];                                  // _FC8
	int mNewIconCurrY[3];                                  // _FD4
	int mEmptyIconInitX[3];                                // _FE0
	int mEmptyIconInitY[3];                                // _FEC
	int mEmptyIconCurrX[3];                                // _FF8
	int mEmptyIconCurrY[3];                                // _1004
	ogCnvStringMgr* mStrCnvDataCorrupt;                    // _1010
	ogCnvStringMgr* mStrCnvConfirmCopy;                    // _1014
	ogCnvStringMgr* mStrCnvFormatComplete;                 // _1018
	ogCnvStringMgr* _101C;                				   // _101C
	ogNitakuMgr* mNitaku;                                  // _1020
	int mMainRootPaneTargetPosX;                           // _1024
	int mPositioningPaneTargetPosX;                        // _1028
	bool mIsLayoutActive;                                  // _102C
	titleMessageFlag mTitleMsg;                            // _1030
	f32 mFadeOutTimer;                                     // _1034
	bool mIsFadingOut;                                     // _1038
	f32 mTitleAnimationTimer;                              // _103C
	bool mIsTitleAnimating;                                // _1040
	f32 _1044;                              			   // _1044
	bool _1048;                                 		   // _1048
	bool mIsDataAnimating;                                 // _1049
	f32 mDataAnimationTimer;                               // _104C
	int mYesNoWindowChoice;                                // _1050
	int mTailEffectCounter;                                // _1054
	P2DScreen* mSlotScreensData[3];                        // _1058
	P2DScreen* mSlotScreensNoData[3];                      // _1064
	P2DScreen* mMainUIScreen;                              // _1070
	P2DScreen* mFileInfoScreen;                            // _1074
	P2DScreen* mCopyCursorsScreen;                         // _1078
	P2DScreen* mBlackOverlayScreen;                        // _107C
	P2DPane* mRootPane;                                    // _1080
	P2DPane* mPosPane;                                     // _1084
	P2DPicture* mCardAccessIcon;                           // _1088
	P2DPicture* mYesNoDialogImage;                         // _108C
	P2DTextBox* mYesNoDialogPromptText;                    // _1090
	P2DPane* mYesNoDialogPane;                             // _1094
	P2DPane* mTitleTextBasePane;                           // _1098
	P2DTextBox* mNitakuYesText;                            // _109C
	P2DTextBox* mNitakuNoText;                             // _10A0
	P2DPane* mPromptSelectSavePane;                        // _10A4
	P2DPane* mPromptNoCardPane;                            // _10A8
	P2DPane* mPromptCardErrPane;                           // _10AC
	P2DPane* mPromptSelectCopySrcPane;                     // _10B0
	P2DPane* mOpInProgressPane;                            // _10B4
	P2DPane* mPromptConfirmDelPane;                        // _10B8
	P2DPane* mOpCompletePane;                              // _10BC
	P2DPane* mPromptSaveFailPane;                          // _10C0
	P2DPane* mPromptSaveOKPane;                            // _10C4
	P2DPane* mPromptCardFullPane;                          // _10C8
	P2DTextBox* mCorruptText;                              // _10CC
	P2DTextBox* mConfirmCopyText;                          // _10D0
	P2DTextBox* mFormatDoneText;                           // _10D4
	P2DTextBox* _10D8;                           		   // _10D8
	P2DTextBox* mAltMsgText1;                              // _10DC
	P2DTextBox* mAltMsgText2;                              // _10E0
	P2DTextBox* mAltMsgText3;                              // _10E4
	P2DTextBox* _10E8;                              	   // _10E8
	P2DPane* mFileInfoPane1;                               // _10EC
	P2DPane* mFileInfoPane2;                               // _10F0
	P2DPane* mFileInfoPane3;                               // _10F4
	P2DPane* mAllFileInfoPane;                             // _10F8
	P2DPane* mPartsTensPane;                               // _10FC
	P2DPane* mPartsUnitsPane;                              // _1100
	P2DPane* mTotalPartsTensPane;                          // _1104
	P2DPane* mTotalPartsUnitsPane;                         // _1108
	P2DPane* mRedPikiHundPane;                             // _110C
	P2DPane* mRedPikiTensPane;                             // _1110
	P2DPane* mRedPikiUnitsPane;                            // _1114
	P2DPane* mBluePikiHundPane;                            // _1118
	P2DPane* mBluePikiTensPane;                            // _111C
	P2DPane* mBluePikiUnitsPane;                           // _1120
	P2DPane* mYellowPikiHundPane;                          // _1124
	P2DPane* mYellowPikiTensPane;                          // _1128
	P2DPane* mYellowPikiUnitsPane;                         // _112C
	P2DPane* mRedPikiInfoPane;                             // _1130
	P2DPane* mBluePikiInfoPane;                            // _1134
	P2DPane* mYellowPikiInfoPane;                          // _1138
	P2DPane* mNavInfoPane;                                 // _113C
	P2DPane* mTopNewDataIconPane;                          // _1140
	P2DPane* mIconNoDataRootPanes[3];                      // _1144
	P2DPane* mIconDataRootPanes[3];                        // _1150
	P2DPane* mIconNewPanes[3];                             // _115C
	P2DPane* mIconEmptyPanes[3];                           // _1168
	P2DPane* mIconOnyonPanes[3];                           // _1174
	P2DPane* mIconPikminPanes[3];                          // _1180
	P2DPane* mIconFxAnchorPanes[3];                        // _118C
	P2DPane* mIconOnyonDestPanes[3];                       // _1198
	P2DPane* mIconPikminDestPanes[3];                      // _11A4
	P2DPane* mDayCount1DigitPane[3];                       // _11B0
	P2DPicture* mDayCount1DigitPic[3];                     // _11BC
	P2DPane* mDayCount2DigitLPane[3];                      // _11C8
	P2DPane* mDayCount2DigitRPane[3];                      // _11D4
	P2DPicture* mDayCount2DigitLPic[3];                    // _11E0
	P2DPicture* mDayCount2DigitRPic[3];                    // _11EC
	P2DPicture* mDayCountColorSrcPic[3];                   // _11F8
	P2DPane* mSlotNormalPane;                              // _1204
	P2DPane* mSlotNewPane;                                 // _1208
	P2DPane* mOperationCursorsScreenPane;                  // _120C
	P2DPicture* mDeleteCursorPicture;                      // _1210
	u8 _UNUSED1214[0x6C];                                  // _1214, unknown
	SpectrumCursorMgr mDeleteLeftCursor;                   // _1280
	u8 _UNUSED12B0[0x10];                                   // _12B0, unknown
	SpectrumCursorMgr mDeleteRightCursor;                  // _12C0
	u8 _UNUSED12F0[0x10];                                  // _12F0, unknown
	f32 mTailEffectMoveTimer;                              // _1300
	u8 _UNUSED1304[0xC];                                   // _1304
	f32 mSelectionChangeTimer;                             // _1310
	Colour mColorNormalFile;                               // _1314
	Colour mColorSelectedFile;                             // _1318
	Colour mColorNoDataFile;                               // _131C
	Colour mColorNoDataSelectedFile;                       // _1320
	u8 _UNUSED1324[0xC20];                                 // _1324, unknown
	u8 mDeleteCursorPictureOpacity;                        // _1F44
	f32 mSelectionConfirmEffectTimer;                      // _1F48
	f32 mTailEffectSpawnTimer;                             // _1F4C
	f32 mCopyAnimTimer;                                    // _1F50
	bool mFileSlotSelectionStates[3];                      // _1F54
	bool mIsTailMoveEffectActive;                          // _1F57
	bool _1194;                                            // _1F58
	bool mCanCreateNewFile;                                // _1F59
	bool mMemoryCardCheckState;                            // _1F5A
	bool mIsCopyingFileActive;                             // _1F5B
	bool mIsCopyCompleteMessageActive;                     // _1F5C
	bool mIsDeletingFileActive;                            // _1F5D
	int mCopyTargetFileIndex;                              // _1F60
	bool mIsCopyTargetSelectionActive;                     // _1F64
};

} // namespace zen

#endif
