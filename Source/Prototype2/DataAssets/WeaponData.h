#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupTypeEnum.h"
#include "WeaponData.generated.h"

/* Needed for PickupType Enum */
class APickUpItem;

/**
 * Data Asset to hold all data for the weapons
 */
UCLASS()
class PROTOTYPE2_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	FName Name;

	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	EPickupDataType PickupType;

	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	UStaticMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category= "Animation")
	UAnimMontage* ChargeAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category= "Animation")
	UAnimMontage* ExecuteAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	float GrowTime;
	
	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	UMaterialInstance* SeedMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category= "Seed")
	UStaticMesh* ParachuteMesh;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* SeedIcon;
		
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* WeaponIcon;
	
	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float WeaponReach;

	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float AOEMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attack")
	int32 Durability = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float Cooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float KnockbackMultiplier = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float MaxKnockback = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float KnockUpMultiplier = 0.3f;
};
