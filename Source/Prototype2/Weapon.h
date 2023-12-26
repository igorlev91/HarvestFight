// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API AWeapon : public APickUpItem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	int numberOfHits;
};
