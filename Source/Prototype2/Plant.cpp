// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Prototype2Character.h"
#include "Kismet/GameplayStatics.h"

APlant::APlant()
{
	bReplicates = true;
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	int x = rand() % 100;
	if (x < chanceOfGold)
	{
		ItemComponent->gold = true;
	}

	if (ItemComponent->gold)
	{
		// change the plant material to gold
		ItemComponent->CropValue *= goldMultiplier;
	}
}


void APlant::Interact(APrototype2Character* player)
{
	if (isGrown)
	{
		ItemComponent->Interact(player, this);
	}
}

void APlant::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	if (!owner->HeldItem)
	{
		_invokingWiget->SetHUDInteractText("Pick Up");
	}
}
