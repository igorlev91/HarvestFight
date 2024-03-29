

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

	if (!HoneyData)
	{
		return;
	}
	
	APlant* Honey = GetWorld()->SpawnActor<APlant>(HoneyPrefab, FVector(0, 0, 0), {});
	Honey->SeedData->PlantData = HoneyData;
	
	TArray<AActor*> NearbyFlowerActors{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlant::StaticClass(), NearbyFlowerActors);
	NumberOfNearbyFlowers = 0;
	for(AActor* FlowerActor : NearbyFlowerActors)
	{
		if (FlowerActor == this)
			continue;
		
		if (FVector::Distance(GetActorLocation(), FlowerActor->GetActorLocation()) <= FlowerEffectionDistance)
		{
			if (APlant* NearbyFlower = Cast<APlant>(FlowerActor))
			{
				if (NearbyFlower->SeedData->BabyType == EPickupDataType::FlowerData)
				{
					NumberOfNearbyFlowers += NearbyFlower->SeedData->BabyStarValue;
				}
			}
		}
	}
	
	Honey->SetSeedData(Honey->SeedData, EPickupActor::PlantActor);
	Honey->NumberOfNearbyFlowers = NumberOfNearbyFlowers;
	Honey->bGrown = true;
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
	//SSComponent->Boing();
}

void ABeehive::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
}

bool ABeehive::IsInteractable(APrototype2PlayerState* _Player)
{
	return bIsReadyToCollect;
}

void ABeehive::SetBeehiveLocation()
{
	if (Bees && ItemComponent->Mesh)
	{
		Bees->SeekPositionLocation = ItemComponent->Mesh->GetComponentLocation();
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
		ItemComponent->Mesh->SetWorldScale3D(FMath::Lerp(SeedData->BabyScale, FVector::One() * 2.0f, TrackerTimeTillCollect / TimeTillCollect));
	

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
