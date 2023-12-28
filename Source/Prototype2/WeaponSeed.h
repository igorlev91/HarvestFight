// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Seed.h"
#include "WeaponSeed.generated.h"

/**
 * 
 */
class AWeapon;
UCLASS()
class PROTOTYPE2_API AWeaponSeed : public ASeed
{
	GENERATED_BODY()
public:
	virtual void Interact(APrototype2Character* player) override;
	virtual void Grow() override;
	virtual void Tick(float DeltaSeconds) override;
	AWeaponSeed();

private:
	UPROPERTY(EditAnywhere)
	AWeapon* weaponToGrow = nullptr;
};
