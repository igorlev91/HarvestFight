

#pragma once

#include "CoreMinimal.h"
#include "FertiliserData.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "PoisonFertiliserData.generated.h"

UCLASS()
class PROTOTYPE2_API UPoisonFertiliserData : public UFertiliserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Properties")
	float SellMultiplier{-0.5f};
};
