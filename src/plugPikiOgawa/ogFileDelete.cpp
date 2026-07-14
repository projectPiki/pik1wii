#include "DebugLog.h"
#include "SoundMgr.h"
#include "gameflow.h"
#include "jaudio/verysimple.h"
#include "zen/EffectMgr2D.h"
#include "zen/ogFileSelect.h"
#include "zen/ogNitaku.h"

#include "floats_small.h"

/**
 * @todo: Documentation
 * @note UNUSED Size: 00009C
 */
DEFINE_ERROR(__LINE__) // Never used in the DLL

/**
 * @todo: Documentation
 * @note UNUSED Size: 0000F4
 */
DEFINE_PRINT(nullptr)

/**
 * @todo: Documentation
 */
void zen::ogScrFileSelectMgr::setOperateMode_Delete()
{
	SetTitleMsg(DataCorrupted);
	OpenYesNoWindow();
	paneOnOffXY(false);
	mCopyLeftCursor.scale(0.0f, 0.001f);
	mCopyRightCursor.scale(0.0f, 0.001f);
}

/**
 * @todo: Documentation
 * @note UNUSED Size: 000178
 */
void zen::ogScrFileSelectMgr::DeleteEffectStart()
{
	SetTitleMsg(NoMemoryCardInserted);
	Vector3f pos1, pos2;

	pos1.set(0.0f, 0.0f, 0.0f);

	pos1.x = mIconOnyonPanes[mCurrSlotIdx]->getPosH() + mIconOnyonPanes[mCurrSlotIdx]->getWidth() / 2.0f;
	pos1.y = 480.0f - (mIconOnyonPanes[mCurrSlotIdx]->getPosV() + mIconOnyonPanes[mCurrSlotIdx]->getHeight() / 2.0f);
	mFxMgr->create(EFF2D_Unk44, pos1, nullptr, nullptr);

	pos2.set(pos1.x, (480.0f - pos1.y) - 80.0f, 0.0f);
	mFxMgr->create(EFF2D_Unk43, pos2, nullptr, nullptr);
	// UNUSED FUNCTION
}

/**
 * @todo: Documentation
 */
void zen::ogScrFileSelectMgr::OperateDelete(Controller* input)
{
	if (mMemoryCardCheckState) {
		mTailEffectSpawnTimer -= gsys->getFrameTime();
		if (mTailEffectSpawnTimer < 0.0f) {
			mMemoryCardCheckState = false;
			setOperateMode(Normal);
		}
		return;
	}

	if (mCanCreateNewFile) {
		mCardAccessIcon->show();
		f32 rate = mTailEffectSpawnTimer;
		int x, y;
		if (rate > 1.7f) {
			y = mCardAccessIcon->getPosV();
			x = (rate - 1.7f) / 0.3f * 640.0f;
		} else if (rate < 0.3f) {
			y = mCardAccessIcon->getPosV();
			x = (rate - 0.3f) / 0.3f * 640.0f;
			mCardAccessIcon->move(x, y);
		} else {
			x = mCardAccessIcon->getPosH();
			y = mCardAccessIcon->getPosV();
		}
		mCardAccessIcon->move(x, y);
		mTailEffectSpawnTimer -= gsys->getFrameTime();

		if (!mIsDeletingFileActive && gameflow.mMemoryCard.hasCardFinished()) {
			mIsDeletingFileActive = true;
			copyCardInfosSub();
			ChkNewData();
			seSystem->playSysSe(SYSSE_CARDOK);
		}
		if (mIsDeletingFileActive && mTailEffectSpawnTimer < 0.0f) {
			mCanCreateNewFile     = false;
			mMemoryCardCheckState = true;
			mTailEffectSpawnTimer = 1.0f;
			mCardAccessIcon->hide();
			SetTitleMsg(MemoryCardErrorOccurred);
		}
		return;
	}

	ogNitakuMgr::NitakuStatus status = mNitaku->update(input);
	if (status >= ogNitakuMgr::Status_4) {
		CloseYesNoWindow();
	}
	if (status == ogNitakuMgr::Status_4) {
		seSystem->playSysSe(SYSSE_CANCEL);
		setOperateMode(Normal);
	} else if (status == ogNitakuMgr::ExitSuccess) {
		seSystem->playSysSe(SYSSE_CARDACCESS);
		gameflow.mMemoryCard.delFile(mCardInfo[mCurrSlotIdx]);
		DeleteEffectStart();
		mTailEffectSpawnTimer = 2.0f;
		mCanCreateNewFile     = true;
		mIsDeletingFileActive = false;
	} else if (status == ogNitakuMgr::ExitFailure) {
		setOperateMode(Normal);
	}
}
