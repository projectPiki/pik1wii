#include "DebugLog.h"
#include "Interactions.h"
#include "teki.h"

/**
 * @todo: Documentation
 * @note UNUSED Size: 00009C
 */
DEFINE_ERROR(__LINE__) // Never used in the DLL

/**
 * @todo: Documentation
 * @note UNUSED Size: 0000F4
 */
DEFINE_PRINT("tekiinteraction");

/**
 * @todo: Documentation
 * @note UNUSED Size: 00000C
 */
TekiInteractionKey::TekiInteractionKey(int type, immut Interaction* interaction)
{
	mInteractionType = type;
	mInteraction     = interaction;
}

/**
 * @todo: Documentation
 */
bool InteractAttack::actTeki(Teki* teki) immut
{
	TekiInteractionKey attack(TekiInteractType::Attack, this);
	return teki->interact(attack);
}

/**
 * @todo: Documentation
 */
bool InteractBomb::actTeki(Teki* teki) immut
{
	f32 bombFactor = teki->getParameterF(TPF_BombDamageRate);
	TekiInteractionKey attack(TekiInteractType::Attack, &InteractAttack(mOwner, nullptr, mDamage * bombFactor, false));
	return teki->interact(attack);
}

/**
 * @todo: Documentation
 */
bool InteractHitEffect::actTeki(Teki* teki) immut
{
	TekiInteractionKey hiteffect(TekiInteractType::HitEffect, this);
	return teki->interact(hiteffect);
}

/**
 * @todo: Documentation
 */
bool InteractSwallow::actTeki(Teki*) immut
{
	return true;
}

/**
 * @todo: Documentation
 */
bool InteractPress::actTeki(Teki* teki) immut
{
	TekiEvent event(TekiEventType::Pressed, teki, mOwner);
	teki->eventPerformed(event);
	return true;
}

/**
 * @todo: Documentation
 */
bool InteractFlick::actTeki(Teki*) immut
{
	return true;
}
