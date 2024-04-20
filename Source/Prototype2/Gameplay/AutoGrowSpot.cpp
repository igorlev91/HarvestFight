
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
}

void AAutoGrowSpot::BeginPlay()
{
	Super::BeginPlay();
	
	ReGrowTimer = ReGrowInterval;
}

void AAutoGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAutoGrowSpot, ReGrowTimer);
}

void AAutoGrowSpot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
		
	if (!HasAuthority())
		return;

	//
	if (ReGrowTimer > 0)
	{
		ReGrowTimer -= DeltaSeconds;
	}
	if (ReGrowTimer <= 0)
	{
		if (IsValid(ItemRef) == false)
		{
			if (auto Gamemode = UGameplayStatics::GetGameMode(GetWorld()))
			{
				if (auto CastedGamemode = Cast<APrototype2GameMode>(Gamemode))
				{
					if (ADataAssetWorldOverride* DataAssetWorldOverride = CastedGamemode->DataAssetWorldOverride)
					{
						ReGrowTimer = ReGrowInterval;
						ASeed* NewSeed = Cast<ASeed>(GetWorld()->SpawnActor(ASeed::StaticClass()));
						
						if (bWeaponOnly)
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

void AAutoGrowSpot::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
	int _PlayerID)
{
	OnDisplayInteractText_Unprotected(_InvokingWidget, _Owner);
}

EInteractMode AAutoGrowSpot::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	return IsInteractable_Unprotected(_Player);
}
