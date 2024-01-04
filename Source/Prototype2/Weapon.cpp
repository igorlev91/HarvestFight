// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "InteractInterface.h"
#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2Character.h"

UWeapon::UWeapon()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	InterfaceType = EInterfaceType::Weapon;
}

void UWeapon::Interact(APrototype2Character* player)
{
}

void UWeapon::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
}

bool UWeapon::IsInteractable(APrototype2PlayerState* player)
{
	return false;
}


