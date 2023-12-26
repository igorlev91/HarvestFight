// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSeed.h"
#include "Weapon.h"

void AWeaponSeed::Grow()
{
	if (growtime <= 0)
	{
		// Spawn Weapon
		// Destroy this
	}
}

void AWeaponSeed::Tick(float DeltaSeconds)
{
	if (isPlanted)
	{
		growtime -= GetWorld()->GetDeltaSeconds();
		Grow();
	}
}
