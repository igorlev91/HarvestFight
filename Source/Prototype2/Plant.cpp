// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

APlant::APlant()
{
	bReplicates = true;
	LeavesMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Leaves Mesh"));
	LeavesMesh->SetupAttachment(RootComponent);

	InterfaceType = EInterfaceType::Default;
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	//Server_ToggleGold();

	LeavesMesh->SetupAttachment(RootComponent);
	LeavesMesh->SetCollisionProfileName(FName("NoCollision"));
}


void APlant::Interact(APrototype2Character* player)
{
	if (isGrown)
	{
		ItemComponent->Interact(player, this);

		player->EnableStencil(false);
		if (player->PlayerHUDRef)
		{
			player->PlayerHUDRef->SetHUDInteractText("");
		}
		ItemComponent->Mesh->SetRenderCustomDepth(false);
		LeavesMesh->SetRenderCustomDepth(false);
	}
}

void APlant::ClientInteract(APrototype2Character* player)
{
	IInteractInterface::ClientInteract(player);

	if (isGrown)
	{
		player->UpdateDecalDirection(true, true);
	}
}

void APlant::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	if (!owner->HeldItem || owner->HeldItem != this)
	{
		_invokingWiget->SetHUDInteractText("Pick Up");

		owner->EnableStencil(true);
	}
}

bool APlant::IsInteractable(APrototype2PlayerState* player)
{
	if (!player)
		return false;

	if (auto controller = player->GetPlayerController())
	{
		if (auto character = controller->GetCharacter())
		{
			if (auto casted = Cast<APrototype2Character>(character))
			{
				if (!casted->HeldItem || casted->HeldItem != this)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

void APlant::Server_ToggleGold_Implementation()
{
	Multi_ToggleGold();
}

void APlant::Multi_ToggleGold_Implementation()
{
	int x = rand() % 100;
	if (x < chanceOfGold)
	{
		ItemComponent->gold = true;
	}

	if (ItemComponent->gold)
	{
		// change the plant material to gold
		if (goldMaterial)
		{
			ItemComponent->Mesh->SetMaterial(0, goldMaterial);
		}
		if (goldMaterial2)
		{
			LeavesMesh->SetMaterial(0, goldMaterial2);
		}
		ItemComponent->CropValue *= goldMultiplier;
	}
}
