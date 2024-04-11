
#include "AutoGrowSpot.h"

#include <Prototype2/DataAssets/ConcreteBagData.h>

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/DataAssetWorldOverride.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/DataAssets/WorldOverrideData.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AAutoGrowSpot::AAutoGrowSpot()
{
	bReplicates = true;
	bIsNormalGrowSpot = false;
}

void AAutoGrowSpot::BeginPlay()
{
	AGrowSpot::BeginPlay();
	
	ReGrowTimer = ReGrowInterval;
}

void AAutoGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AGrowSpot::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAutoGrowSpot, ReGrowTimer);
}

void AAutoGrowSpot::Tick(float DeltaSeconds)
{
	AGrowSpot::Tick(DeltaSeconds);
		
	if (!HasAuthority())
		return;

	//
	if (ReGrowTimer > 0)
	{
		ReGrowTimer -= DeltaSeconds;
	}
	if (ReGrowTimer <= 0)
	{
		if (!GrowingItemRef)
		{
			if (auto Gamemode = UGameplayStatics::GetGameMode(GetWorld()))
			{
				if (auto CastedGamemode = Cast<APrototype2GameMode>(Gamemode))
				{
					if (ADataAssetWorldOverride* DataAssetWorldOverride = CastedGamemode->DataAssetWorldOverride)
					{
						ReGrowTimer = ReGrowInterval;
						ASeed* NewSeed = Cast<ASeed>(GetWorld()->SpawnActor(ASeed::StaticClass()));
						if (rand() % 3 == 0)
							NewSeed->ServerData.SeedData = DataAssetWorldOverride->WorldOverrideData->WeaponSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->WeaponSeeds.Num()];
						else
							NewSeed->ServerData.SeedData = DataAssetWorldOverride->WorldOverrideData->PlantSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->PlantSeeds.Num()];
						PlantASeed(NewSeed);
					}
				}
			}
		}
	}

	
}

bool AAutoGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	return IsInteractable_Unprotected(_Player);
}
