
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
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
};
