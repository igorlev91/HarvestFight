
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
	
	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				switch(GrowSpotState)
				{
				case EGrowSpotState::Empty:
					{
						if (Cast<ASeed>(Casted->HeldItem))
						{
							return true;
						}
						if (Cast<AFertiliser>(Casted->HeldItem) && !bIsFertilised)
						{
							return true;
						}
						break;
					}
				case EGrowSpotState::Growing:
					{
						if (Cast<AFertiliser>(Casted->HeldItem) && !Weapon)
							return true;
						break;
					}
				case EGrowSpotState::Grown:
					{
						return true;
					}
				default:
					break;
				}
			}
		}
	}
	

	return false;
}
