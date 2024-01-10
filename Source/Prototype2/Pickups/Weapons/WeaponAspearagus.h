
#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Weapon.h"
#include "WeaponAspearagus.generated.h"

UCLASS()
class PROTOTYPE2_API UWeaponAspearagus : public UWeapon
{
	GENERATED_BODY()
public:
	/* Allow rotation while player isn't moving for aiming purposes */
	virtual void ChargeAttack(APrototype2Character* _Player) override;
	
	/* Play full charge or normal attack animation */
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player) override;
	
	/* Any weapon specific functionality while executing attack*/
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player) override;
	
	/* How this weapon affects the AOE indicator */
	virtual void UpdateAOEIndicator(APrototype2Character* _Player) override;
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(APrototype2Character* _Player, float _AttackSphereRadius);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SpawnProjectile(APrototype2Character* _Player, float _AttackSphereRadius);
};
