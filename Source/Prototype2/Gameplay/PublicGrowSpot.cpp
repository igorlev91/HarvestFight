
#include "PublicGrowSpot.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

APublicGrowSpot::APublicGrowSpot()
{
	bReplicates = true;
}

bool APublicGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	return IsInteractable_Unprotected(_Player);
}
