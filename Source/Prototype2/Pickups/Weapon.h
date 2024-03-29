#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "ItemComponent.h"
#include "Weapon.generated.h"

class APickUpItem;
UCLASS()
class PROTOTYPE2_API UWeapon : public UActorComponent
{
public:
	GENERATED_BODY()
	UWeapon();
	/* Attacking functions*/

	/* For weapon specific charging functionality */
	virtual void Client_ChargeAttack(APrototype2Character* _Player){}
	virtual void ChargeAttack(APrototype2Character* _Player){}

	/* When player is hit while charging*/
	virtual void Client_ChargeAttackCancelled(APrototype2Character* _Player){}
	virtual void ChargeAttackCancelled(APrototype2Character* _Player);
	
	/* Play full charge or normal attack animation */
	UFUNCTION(Client, Reliable)
	virtual void Client_ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player);
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player);

	/* Any weapon specific functionality while executing attack*/
	virtual void Client_ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, float _AttackChargeAmount){}
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, float _AttackChargeAmount, bool bIsSprinting);

	/* How this weapon affects the AOE indicator */
	virtual void Client_UpdateAOEIndicator(APrototype2Character* _Player, float _AttackChargeAmount){}
	virtual void UpdateAOEIndicator(APrototype2Character* _Player, float _AttackChargeAmount){}

	UFUNCTION(Client, Reliable)
	void Client_BroadcastAttackToHUD(APrototype2Character* _Player);

	void CheckIfCrownHit(APrototype2Character* _Attacker, APrototype2Character* _Victim);

	UFUNCTION(Client, Reliable)
	void Client_SetFollowingThroughToFalse(APrototype2Character* _Player);
};
