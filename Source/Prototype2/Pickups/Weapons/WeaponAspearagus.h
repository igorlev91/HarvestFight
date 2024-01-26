
#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Weapon.h"
#include "WeaponAspearagus.generated.h"

UCLASS()
class PROTOTYPE2_API UWeaponAspearagus : public UWeapon
{
	GENERATED_BODY()
public:
	virtual void Client_ChargeAttack(APrototype2Character* _Player) override;
	virtual void ChargeAttack(APrototype2Character* _Player) override;

	/* When player is hit while charging*/
	virtual void Client_ChargeAttackCancelled(APrototype2Character* _Player) override;
	virtual void ChargeAttackCancelled(APrototype2Character* _Player) override;
	
	/* Play full charge or normal attack animation */
	virtual void Client_ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player) override;
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player) override;

	/* Any weapon specific functionality while executing attack*/
	virtual void Client_ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector) override;
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector) override;

	/* How this weapon affects the AOE indicator */
	virtual void Client_UpdateAOEIndicator(APrototype2Character* _Player) override;
	virtual void UpdateAOEIndicator(APrototype2Character* _Player) override;
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(APrototype2Character* _Player, float _AttackSphereRadius);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SpawnProjectile(APrototype2Character* _Player, float _AttackSphereRadius);
};
