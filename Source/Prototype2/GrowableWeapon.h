// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "GrowableWeapon.generated.h"

class UWeapon;
UCLASS()
class PROTOTYPE2_API AGrowableWeapon : public APlant
{
	GENERATED_BODY()
public:
	//UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//UWeapon* Weapon;
	AGrowableWeapon();
	//virtual  void BeginPlay() override;
	virtual void Interact(APrototype2Character* player) override;
	//virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;
};
