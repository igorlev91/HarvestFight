
#include "PublicGrowSpot.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

APublicGrowSpot::APublicGrowSpot()
{
	bReplicates = true;
}

bool APublicGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;

	APlayerController* Controller = _Player->GetPlayerController();
	if (!Controller)
		return false;

	ACharacter* Character = Controller->GetCharacter();
	if (!Character)
		return false;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(Character);
	if (!CastedCharacter)
		return false;

	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return true;
			if (Cast<AFertiliser>(CastedCharacter->HeldItem) && !bIsFertilised)
				return true;
			
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (Cast<AFertiliser>(CastedCharacter->HeldItem))
				return true;
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (Cast<AFertiliser>(CastedCharacter->HeldItem))
				return true;
		
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return false;
			
			if (ABeehive* SomeBeehive = Cast<ABeehive>(GrowingActor))
				return SomeBeehive->IsInteractable(_Player);

			return true;
		}
	default:
		{
			break;
		}
	}

	return false;
}
