
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Appearance")
	FName Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Type")
	EPickupDataType Type;
	UPROPERTY(EditDefaultsOnly, Category= "Value")
	int32 StarValue;
	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	float GrowTime;
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	UStaticMesh* Mesh;
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	TArray<UMaterialInstance*> Materials;
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	UStaticMesh* BabyMesh;
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	TArray<UMaterialInstance*> BabyMaterials;
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	FVector BabyScale = {2.0f, 2.0f, 2.0f};
	
	UPROPERTY(EditDefaultsOnly, Category= "Gold Appearance")
	TArray<UMaterialInstance*> BabyGoldMaterials;
	UPROPERTY(EditDefaultsOnly, Category= "Gold Appearance")
	int32 GoldMultiplier = 3;
	UPROPERTY(EditDefaultsOnly, Category= "Gold Appearance")
	int32 ChanceOfGold = 5;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Weapon Baby")
	class UWeaponData* WeaponData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Plant Baby")
	class UPlantData* PlantData;
};
