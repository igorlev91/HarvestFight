// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GrowableSellable.generated.h"

/**
 * Data Asset to hold all data for the growable and sellable objects,
 * everything it will need from spawn to sell (seed packet mesh,
 * plant mesh, value, grow time etc.
 */
UCLASS()
class PROTOTYPE2_API UGrowableSellable : public UDataAsset
{
	GENERATED_BODY()
protected:
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<class ASeed> Seed;
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<class APlant> Plant;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* PlantMesh;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* SeedMesh;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Stars;

	UPROPERTY(EditDefaultsOnly)
	float GrowTime;
	
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PickupIcon;
	
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* GoldPickupIcon;
};
