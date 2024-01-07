#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmPlot.generated.h"

class AGrowSpot;
UCLASS()
class PROTOTYPE2_API AFarmPlot : public AActor
{
	GENERATED_BODY()

	AFarmPlot();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
};
