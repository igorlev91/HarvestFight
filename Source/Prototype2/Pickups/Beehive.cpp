

#include "Prototype2/Pickups/Beehive.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "WhatTheFlock/Public/Flock.h"

void ABeehive::BeginPlay()
{
	Super::BeginPlay();

	if (BeesPrefab)
	{
		Bees = GetWorld()->SpawnActor<AFlock>(BeesPrefab, FVector(0, 0, 0), {});
		Bees->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
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
	APlant* Honey = GetWorld()->SpawnActor<APlant>(Plant, FVector(0, 0, 0), {});
	Honey->PlantData = HoneyData;
	Honey->SetPlantData(Honey->PlantData);
	Honey->bGrown = true;
	Honey->NumberOfNearbyFlowers = NumberOfNearbyFlowers;
	Honey->Multi_ScalePlant();
	
	Honey->Interact(_Player);

	bIsReadyToCollect = false;
	TrackerTimeTillCollect = 0;
}

void ABeehive::ClientInteract(APrototype2Character* _Player)
{
	Super::ClientInteract(_Player);
}

void ABeehive::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	Super::OnDisplayInteractText(_InvokingWidget, _Owner, _PlayerID);
}

bool ABeehive::IsInteractable(APrototype2PlayerState* _Player)
{
	return Super::IsInteractable(_Player);
}

void ABeehive::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsReadyToCollect)
		return;
	
	TrackerTimeTillCollect += DeltaSeconds;

	if (TrackerTimeTillCollect > TimeTillCollect)
	{
		bIsReadyToCollect = true;
	}
}
