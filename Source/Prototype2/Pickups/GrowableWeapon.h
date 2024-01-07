
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
	virtual void Interact(APrototype2Character* _Player) override;
	//virtual void OnDisplayInteractText(class UWidget_PlayerHUD* InvokingWidget, class APrototype2Character* Owner, int _PlayerID) override;
	
private:
	AGrowableWeapon();
	
};
