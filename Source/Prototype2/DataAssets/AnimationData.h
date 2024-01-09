
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimationData.generated.h"

/* Enum for choosing the correct attack animations */
UENUM(BlueprintType)
enum class EWeaponAnimation : uint8
{
	Punching,
	Leek,
	Aspearagus,
};

UCLASS()
class PROTOTYPE2_API UAnimationData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* SkeletalMesh;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* Idle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* Run;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* IdleWithWeapon;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* RunWithWeapon;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* HoldingItem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Pickup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* Sprint;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* SprintHoldingItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequence* SprintWithWeapon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Punching" )
	UAnimSequence* ChargingPunchingAttack;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Punching")
	UAnimMontage* FullChargePunchingAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Punching")
	UAnimMontage* NormalPunchingAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leek" )
	UAnimSequence* ChargingLeekAttack;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leek")
	UAnimMontage* FullChargeLeekAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leek")
	UAnimMontage* NormalLeekAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aspearagus")
	UAnimSequence* ChargingAspearagusAttack;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aspearagus")
	UAnimMontage* FullChargeAspearagusAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aspearagus")
	UAnimMontage* NormalAspearagusAttack;
};
