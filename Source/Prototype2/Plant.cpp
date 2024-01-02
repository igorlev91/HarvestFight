// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Prototype2Character.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

APlant::APlant()
{
	bReplicates = true;
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	Server_ToggleGold();
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

void APlant::Server_ToggleGold_Implementation()
{
	Multi_ToggleGold();
}

void APlant::Multi_ToggleGold_Implementation()
{
	/*int x = rand() % 100;
	if (x < chanceOfGold)
	{
		
	}*/

	ItemComponent->gold = true;
	

	if (ItemComponent->gold)
	{
		// change the plant material to gold
		if (goldMaterial)
		{
			ItemComponent->Mesh->SetMaterial(0, goldMaterial);
		}
		if (goldMaterial2)
		{
			//ItemComponent->Mesh->SetMaterial(0, goldMaterial2);
		}
		ItemComponent->CropValue *= goldMultiplier;
	}
}
