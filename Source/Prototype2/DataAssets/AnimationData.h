
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimationData.generated.h"


UCLASS()
class PROTOTYPE2_API UAnimationData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly)
	UClass* TemplatedAnimationBlueprint;
		
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* FallOnButtAndGetBackUp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Launch")
	UAnimSequence* Launch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Launch")
	UAnimSequence* Apex;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Launch")
	UAnimSequence* Falling;
	
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
				
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HoneyStick")
	UAnimSequence* ChargingHoneyStickAttack;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HoneyStick")
	UAnimMontage* FullChargeHoneyStickAttack;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HoneyStick")
	UAnimMontage* NormalHoneyStickAttack;
};
