
#pragma once

#include "CoreMinimal.h"
#include "GrowSpot.h"
#include "PublicGrowSpot.generated.h"

UCLASS()
class PROTOTYPE2_API APublicGrowSpot : public AGrowSpot
{
	GENERATED_BODY()

public:
	APublicGrowSpot();
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player) override;
};
