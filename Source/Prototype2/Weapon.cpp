// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "EdGraphSchema_K2_Actions.h"
#include "Prototype2Character.h"
void AWeapon::Interact(APrototype2Character* player)
{
	//ItemComponent->Interact(player, this);

	/* To move to ItemComponent - move the EInterfaceType to ItemComponent too */
	
	// Setup Mesh
	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Set players HeldItem
	player->Weapon = this;
	
	// Attach to socket
	this->AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHolsterSocket"));

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("Weapon attached to WeaponHolsterSocket"));	
}

AWeapon::AWeapon()
{
	
}
