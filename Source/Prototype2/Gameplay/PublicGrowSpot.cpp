
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
	bIsNormalGrowSpot = false;
}

bool APublicGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{

	return IsInteractable_Unprotected(_Player);
}