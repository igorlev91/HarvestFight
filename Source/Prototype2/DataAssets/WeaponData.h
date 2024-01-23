
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupTypeEnum.h"
#include "WeaponData.generated.h"

/* Needed for PickupType Enum */
class APickUpItem;
class UWeapon;
class USoundCue;
/**
 * Data Asset to hold all data for the weapons
 */
UCLASS()
class PROTOTYPE2_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	EWeaponAnimation WeaponAnimationType;
	
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	USoundCue* AttackAudio;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* WeaponIcon;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	UTexture2D* GoldWeaponIcon;
	
	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float WeaponReach;
	
	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float BaseAttackRadius = 75.0f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float AOEMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attack")
	int32 Durability = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attack")
	float GoldDurabilityMultiplier = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Attack")
	float Cooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float KnockbackMultiplier = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float MaxKnockback = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category= "Knockback")
	float KnockUpMultiplier = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category= "Functionality")
	TSubclassOf<UWeapon> WeaponComponent;
	
	UPROPERTY(EditDefaultsOnly, Category= "Functionality")
	UStaticMesh* AOEIndicatorMesh;

	/* 1.1 = 10% bigger, so the actual hit zone is larger than what
	 * the indicator shows.
	 */
	UPROPERTY(EditDefaultsOnly, Category= "Functionality")
	float ScaleOfAOELargerThanIndicator = 1.1f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Functionality")
	EDebuff Debuff = EDebuff::None;
};
