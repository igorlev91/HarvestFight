// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupTypeEnum.h"
#include "PlantData.generated.h"

/**
 * Data Asset to hold all data for the growable and sellable objects,
 * everything it will need from spawn to sell (seed packet mesh,
 * plant mesh, value, grow time etc.
 */
UCLASS()
class PROTOTYPE2_API UPlantData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category= "Flower")
	float Multiplier = 1.0f;
		
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* PlantIcon;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* GoldPlantIcon;
};
