// Fill out your copyright notice in the Description page of Project Settings.


#include "SellBin.h"

#include "Plant.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerState.h"
#include "GameFramework/PlayerState.h"

// Sets default values
ASellBin::ASellBin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

	InterfaceType = EInterfaceType::SellBin;
}

// Called when the game starts or when spawned
void ASellBin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASellBin::Interact(APrototype2Character* player)
{
	if (player->HeldItem)
	{
		if (auto* plant = Cast<APlant>(player->HeldItem))
		{
			Cast<APrototype2PlayerState>(player->GetPlayerState())->Coins += plant->ItemComponent->CropValue;
			player->HeldItem->Destroy();
			player->HeldItem = nullptr;

		}
	}
}

void ASellBin::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	if(auto heldItem = owner->HeldItem)
	{
		if (heldItem->ItemComponent->PickupType == EPickup::Cabbage ||
			heldItem->ItemComponent->PickupType == EPickup::Carrot ||
			heldItem->ItemComponent->PickupType == EPickup::Mandrake)
		{
			_invokingWiget->SetHUDInteractText("Sell");
		}
	}
}

