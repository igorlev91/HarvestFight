
#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Weapon.h"
#include "WeaponHoneyStick.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWeaponHoneyStick : public UWeapon
{
	GENERATED_BODY()
public:
	
	/* Play full charge or normal attack animation */
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player) override;
	
	/* Any weapon specific functionality while executing attack*/
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player,  float _AttackChargeAmount, bool bIsSprinting) override;
	
	/* How this weapon affects the AOE indicator */
	virtual void UpdateAOEIndicator(APrototype2Character* _Player, float _AttackChargeAmount) override;
};
