// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "GrowableWeapon.generated.h"

class UWeapon;
UCLASS()
class PROTOTYPE2_API AGrowableWeapon : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWeapon* Weapon;
	AGrowableWeapon();
	virtual void Interact(APrototype2Character* player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;
};
