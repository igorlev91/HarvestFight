
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Prototype2/DataAssets/WeaponData.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "SeedData.generated.h"

UCLASS()
class PROTOTYPE2_API USeedData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Identification")
	FName Name;
	UPROPERTY(EditDefaultsOnly, Category= "Packet Appearance")
	class UStaticMesh* PacketMesh;
	UPROPERTY(EditDefaultsOnly, Category= "Packet Appearance")
	TArray<class UMaterialInstance*> PacketMaterials;
	UPROPERTY(EditDefaultsOnly, Category= "Packet UI")
	class UTexture2D* PacketIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Baby")
	EPickupDataType BabyType;
	UPROPERTY(EditDefaultsOnly, Category= "Baby")
	int32 BabyStarValue;
	UPROPERTY(EditDefaultsOnly, Category= "Baby")
	float BabyGrowTime;
	UPROPERTY(EditDefaultsOnly, Category= "Baby Appearance")
	class UStaticMesh* BabyMesh;
	UPROPERTY(EditDefaultsOnly, Category= "Baby Appearance")
	TArray<class UMaterialInstance*> BabyMaterials;
	UPROPERTY(EditDefaultsOnly, Category= "Baby Appearance")
	FVector BabyScale = {2.0f, 2.0f, 2.0f};

	UPROPERTY(EditDefaultsOnly, Category= "Gold")
	int32 GoldMultiplier = 3;
	UPROPERTY(EditDefaultsOnly, Category= "Gold")
	int32 ChanceOfGold = 5;
	UPROPERTY(EditDefaultsOnly, Category= "Gold Appearance")
	TArray<class UMaterialInstance*> BabyGoldMaterials;

	UPROPERTY(EditDefaultsOnly, Category= "Concrete Appearance")
	TArray<class UMaterialInstance*> BabyConcreteMaterials;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Weapon Data")
	class UWeaponData* WeaponData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Plant Data")
	class UPlantData* PlantData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Fertiliser Data")
	class UFertiliserData* FertiliserData;
};
