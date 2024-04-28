

#include "Prototype2/Pickups/Beehive.h"

#include "Boid.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "WhatTheFlock/Public/Flock.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/VFX/SquashAndStretch.h"

ABeehive::ABeehive()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ABeehive::BeginPlay()
{
	Super::BeginPlay();

	if (BeesPrefab)
	{
		if (HasAuthority())
		Bees = GetWorld()->SpawnActor<AFlock>(BeesPrefab);
	}
	TrackerTimeTillCollect = TimeTillCollect;
}

void ABeehive::Interact(APrototype2Character* _Player)
{
	if (!bIsReadyToCollect)
	{
		return;
	}

	if (!HoneySeedData)
	{
		return;
	}
	
	APlant* Honey = GetWorld()->SpawnActor<APlant>(HoneyPrefab, FVector(0, 0, 0), {});
	Honey->ServerData.SeedData = HoneySeedData;
	
	Honey->SetSeedData(Honey->ServerData.SeedData, EPickupActor::PlantActor);
	Honey->NumberOfNearbyFlowers = GetCloseFlowers().Num();
	Honey->Multi_ScalePlant();
	Honey->Interact(_Player);

	bIsReadyToCollect = false;
	TrackerTimeTillCollect = TimeTillCollect;

	if (++HarvestedHoney >= MaxHarvestableHoney)
	{
		if (ParentGrowSpot)
		{
			Multi_DestroyBees();

			ParentGrowSpot->DestroyPlant();
		}
	}
}

void ABeehive::ClientInteract(APrototype2Character* _Player)
{
	if (IsValid(ParentGrowSpot) == false)
		return;
	
	ParentGrowSpot->SetPlantReadySparkle(false, true);
}

void ABeehive::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
}

EInteractMode ABeehive::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	if (bIsReadyToCollect)
	{
		return _ForcedMode != INVALID ? _ForcedMode : INSTANT;
	}

	return INVALID;
}

void ABeehive::SetBeehiveLocation(FVector _Location)
{
	if (HasAuthority())
		Server_SetBeehiveLocation(_Location);
}

TArray<APlant*> ABeehive::GetCloseFlowers()
{
	TArray<AActor*> NearbyFlowerActors{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlant::StaticClass(), NearbyFlowerActors);
	TArray<APlant*> NearbyFlowers{};
	for(AActor* FlowerActor : NearbyFlowerActors)
	{
		if (FlowerActor == this)
			continue;
		
		if (FVector::Distance(GetActorLocation(), FlowerActor->GetActorLocation()) <= FlowerEffectionDistance)
		{
			if (APlant* NearbyFlower = Cast<APlant>(FlowerActor))
			{
				if (NearbyFlower->ServerData.SeedData->BabyType == EPickupDataType::FlowerData)
				{
					NearbyFlowers.Add(NearbyFlower);
				}
			}
		}
	}

	return NearbyFlowers;
}

TArray<FFlowerData> ABeehive::GetCloseFlowerDetails()
{
	int32 NumberOfOneStars{};
	int32 NumberOfTwoStars{};
	int32 NumberOfThreeStars{};
	
	for(auto CloseFlower : GetCloseFlowers())
	{
		if (auto SeedData = CloseFlower->GetSeedData())
		{
			switch (SeedData->BabyStarValue)
			{
			case 1:
				{
					NumberOfOneStars++;
					break;
				}
			case 2:
				{
					NumberOfTwoStars++;
					break;
				}
			case 3:
				{
					NumberOfThreeStars++;
					break;
				}
			default:
				break;
			}
		}
	}

	TArray<FFlowerData> NearbyFlowerDetails{};
	if (NumberOfOneStars > 0)
		NearbyFlowerDetails.Add({1, NumberOfOneStars});
	if (NumberOfTwoStars > 0)
		NearbyFlowerDetails.Add({2, NumberOfTwoStars});
	if (NumberOfThreeStars > 0)
		NearbyFlowerDetails.Add({3, NumberOfThreeStars});

	return NearbyFlowerDetails;
}

void ABeehive::Server_SetBeehiveLocation_Implementation(FVector _Location)
{
	if (Bees && ItemComponent->Mesh)
	{
		Bees->SeekPositionLocation = _Location;
	}
}

void ABeehive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeehive, bIsReadyToCollect);
	DOREPLIFETIME(ABeehive, TrackerTimeTillCollect);
	DOREPLIFETIME(ABeehive, HarvestedHoney)
	DOREPLIFETIME(ABeehive, ParentGrowSpot);
}

void ABeehive::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TimeTillCollect > 0)
		ItemComponent->Mesh->SetWorldScale3D(FMath::Lerp(ServerData.SeedData->BabyScale, FVector::One() * 2.0f, TrackerTimeTillCollect / TimeTillCollect));

	if (bIsReadyToCollect && TimeTillCollect > 0)
	{
		if (IsValid(ParentGrowSpot))
			ParentGrowSpot->SetPlantReadySparkle(true, true);
	}
	else
	{
		if (IsValid(ParentGrowSpot))
			ParentGrowSpot->SetPlantReadySparkle(false, true);
	}
	
	if (!HasAuthority())
		return;
	/*
	TArray<AActor*> NearbyFlowerActors{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlant::StaticClass(), NearbyFlowerActors);
	TArray<APlant*> NearbyFlowers{};
	for(AActor* FlowerActor : NearbyFlowerActors)
	{
		if (FlowerActor == this)
			continue;
		
		if (FVector::Distance(GetActorLocation(), FlowerActor->GetActorLocation()) <= FlowerEffectionDistance)
		{
			if (APlant* NearbyFlower = Cast<APlant>(FlowerActor))
			{
				if (NearbyFlower->SeedData->Type == EPickupDataType::FlowerData)
					NearbyFlowers.Add(NearbyFlower);
			}
		}
	}
		*/
	
	if (TrackerTimeTillCollect > 0)
	{
		TrackerTimeTillCollect -= DeltaSeconds;

		if (TrackerTimeTillCollect <= 0)
		{
			bIsReadyToCollect = true;
		}
	}


}

void ABeehive::Multi_DestroyBees_Implementation()
{
	if (Bees)
	{
		for(auto Bee : Bees->Flock)
		{
			Bee.Boid->Destroy();
		}
		Bees->Destroy();
	}
}
