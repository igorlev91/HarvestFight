#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "ItemComponent.h"
#include "Weapon.generated.h"

class APickUpItem;
UCLASS()
class PROTOTYPE2_API UWeapon : public UActorComponent, public IInteractInterface
{
public:
	GENERATED_BODY()
	UWeapon();

	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;

	/* Attacking functions*/

	/* For weapon specific charging functionality */
	virtual void ChargeAttack(APrototype2Character* _Player);

	/* When player is hit while charging*/
	virtual void ChargeAttackCancelled(APrototype2Character* _Player);
	
	/* Play full charge or normal attack animation */
	virtual void ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player);

	/* Any weapon specific functionality while executing attack*/
	virtual void ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player);

	/* How this weapon affects the AOE indicator */
	virtual void UpdateAOEIndicator(APrototype2Character* _Player);
};
