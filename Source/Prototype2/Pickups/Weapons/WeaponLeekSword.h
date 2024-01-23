
#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Weapon.h"
#include "WeaponLeekSword.generated.h"

UCLASS()
class PROTOTYPE2_API UWeaponLeekSword : public UWeapon
{
	GENERATED_BODY()
public:
	
	/* Play full charge or normal attack animation */
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player) override;
	
	/* Any weapon specific functionality while executing attack*/
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector) override;
	
	/* How this weapon affects the AOE indicator */
	virtual void UpdateAOEIndicator(APrototype2Character* _Player) override;
};
