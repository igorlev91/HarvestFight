// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowableWeapon.h"
#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2Character.h"

AGrowableWeapon::AGrowableWeapon()
{
	Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
	Weapon->Mesh->SetSimulatePhysics(false);
	Weapon->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void AGrowableWeapon::Interact(APrototype2Character* player)
{
	// attach the mesh to the player
}

void AGrowableWeapon::OnDisplayInteractText(UWidget_PlayerHUD* _invokingWiget, APrototype2Character* owner,
	int _playerID)
{
	if (!owner->HeldItem)
	{
		_invokingWiget->SetHUDInteractText("Pick Up");
	}
}
