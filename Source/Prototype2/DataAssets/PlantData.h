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
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	FName Name;

	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	EPickupDataType PickupType;
	
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	UStaticMesh* PlantMesh;
	
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	FVector PlantScale = {2.0f, 2.0f, 2.0f};	
	
	UPROPERTY(EditDefaultsOnly, Category= "Value")
	int32 StarValue;

	UPROPERTY(EditDefaultsOnly, Category= "Value")
	int32 SellValue;
	
	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	float GrowTime;

	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	UMaterialInstance* SeedMaterial;

	UPROPERTY(EditDefaultsOnly, Category= "Gold")
	TArray<UMaterialInstance*> GoldMaterials;

	UPROPERTY(EditDefaultsOnly, Category= "Gold")
	int32 GoldMultiplier = 3;

	UPROPERTY(EditDefaultsOnly, Category= "Gold")
	int32 ChanceOfGold = 5; /* x/100 */
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* SeedIcon;
		
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* PlantIcon;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* GoldPlantIcon;

};
