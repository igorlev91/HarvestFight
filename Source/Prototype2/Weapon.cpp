// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2Character.h"

UWeapon::UWeapon()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void UWeapon::Interact(APrototype2Character* player)
{
	//ItemComponent->Interact(player, this);

	/* To move to ItemComponent - move the EInterfaceType to ItemComponent too */
	
	// Setup Mesh
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Set players HeldItem
	player->Weapon = this;
	
	// Attach to socket
	Mesh->AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHolsterSocket"));

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("Weapon attached to WeaponHolsterSocket"));	
}

void UWeapon::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
}


