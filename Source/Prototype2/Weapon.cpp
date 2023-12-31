// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Prototype2Character.h"
void AWeapon::Interact(APrototype2Character* player)
{
	ItemComponent->Interact(player, this);
}

AWeapon::AWeapon()
{
	
}
