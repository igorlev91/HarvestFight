
#include "PublicGrowSpot.h"

#include <Prototype2/DataAssets/ConcreteBagData.h>

#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"

APublicGrowSpot::APublicGrowSpot()
{
	bReplicates = true;
}

void APublicGrowSpot::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
	int32 _PlayerID)
{
	OnDisplayInteractText_Unprotected(_InvokingWidget, _Owner);
}

EInteractMode APublicGrowSpot::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	return IsInteractable_Unprotected(_Player);
}
