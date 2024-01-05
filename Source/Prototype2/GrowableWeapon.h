.
* File Name : GrowableWeapon.h
* Description : The class for the growable weapon object, used to seperate from other pickups
.

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
	//virtual void OnDisplayInteractText(class UWidget_PlayerHUD* InvokingWidget, class APrototype2Character* Owner, int _PlayerID) override;
};
